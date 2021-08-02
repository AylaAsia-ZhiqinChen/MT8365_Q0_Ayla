/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/compiler/xla/service/gpu/cudnn_conv_algorithm_picker.h"
#include "google/protobuf/any.pb.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/time/time.h"
#include "absl/types/optional.h"
#include "tensorflow/compiler/xla/literal_util.h"
#include "tensorflow/compiler/xla/service/gpu/backend_configs.pb.h"
#include "tensorflow/compiler/xla/service/gpu/buffer_comparator.h"
#include "tensorflow/compiler/xla/service/gpu/convolution_thunk.h"
#include "tensorflow/compiler/xla/service/gpu/gpu_autotuning.pb.h"
#include "tensorflow/compiler/xla/service/gpu/ir_emission_utils.h"
#include "tensorflow/compiler/xla/service/gpu/scratch_allocator.h"
#include "tensorflow/compiler/xla/service/hlo_casting_utils.h"
#include "tensorflow/core/lib/strings/numbers.h"
#include "tensorflow/core/platform/logger.h"
#include "tensorflow/core/platform/mutex.h"
#include "tensorflow/core/util/proto/proto_utils.h"

namespace xla {
namespace gpu {
namespace {

using absl::optional;
using se::DeviceMemoryBase;
using se::dnn::AlgorithmDesc;
using tensorflow::AutotuneResult;

std::vector<AlgorithmDesc> GetAlgorithms(CudnnConvKind kind,
                                         se::StreamExecutor* stream_exec) {
  std::vector<AlgorithmDesc> algorithms;
  bool succ = false;
  switch (kind) {
    case CudnnConvKind::kBackwardFilter:
      succ =
          stream_exec->GetConvolveBackwardFilterAlgorithms(true, &algorithms);
      break;
    case CudnnConvKind::kBackwardInput:
      succ = stream_exec->GetConvolveBackwardDataAlgorithms(true, &algorithms);
      break;
    case CudnnConvKind::kForward:
    case CudnnConvKind::kForwardActivation:
      succ = stream_exec->GetConvolveAlgorithms(true, &algorithms);
      break;
  }
  DCHECK(succ);

  return algorithms;
}

string AlgorithmToString(const AlgorithmDesc& algo) {
  if (algo.tensor_ops_enabled()) {
    return absl::StrCat(algo.algo_id(), "+TC");
  }
  return absl::StrCat(algo.algo_id());
}

string NumBytesToString(int64 bytes) {
  return absl::StrCat(tensorflow::strings::HumanReadableNumBytes(bytes), " (",
                      bytes, "B)");
}

// Acquires a process-global lock on the device pointed to by the given
// StreamExecutor.
//
// This is used to prevent other XLA instances from trying to autotune on this
// device while we're using it.
tensorflow::mutex_lock LockGpu(const se::StreamExecutor* stream_exec) {
  static tensorflow::mutex mu(tensorflow::LINKER_INITIALIZED);
  // se::Platform*s are global singletons guaranteed to live forever.
  static auto* mutexes =
      new std::map<std::pair<const se::Platform*, /*device_ordinal*/ int64>,
                   tensorflow::mutex>();

  tensorflow::mutex_lock global_lock(mu);
  auto it = mutexes
                ->emplace(std::piecewise_construct,
                          std::make_tuple(stream_exec->platform(),
                                          stream_exec->device_ordinal()),
                          std::make_tuple())
                .first;
  return tensorflow::mutex_lock{it->second};
}

tensorflow::CudnnVersion GetCudnnVersion(se::StreamExecutor* stream_executor) {
  tensorflow::CudnnVersion cudnn_version;
  if (auto* dnn = stream_executor->AsDnn()) {
    StatusOr<se::dnn::VersionInfo> version_or = dnn->GetVersion();
    if (version_or.ok()) {
      const auto& version = version_or.ValueOrDie();
      cudnn_version.set_major(version.major_version());
      cudnn_version.set_minor(version.minor_version());
      cudnn_version.set_patch(version.patch());
    }
  }
  return cudnn_version;
}

tensorflow::ComputeCapability GetComputeCapability(
    se::StreamExecutor* stream_executor) {
  tensorflow::ComputeCapability cc;
  int cc_major, cc_minor;
  stream_executor->GetDeviceDescription().cuda_compute_capability(&cc_major,
                                                                  &cc_minor);
  cc.set_major(cc_major);
  cc.set_minor(cc_minor);
  return cc;
}

}  // anonymous namespace

// We could have caching here so that we don't redo this work for two identical
// convolutions.  Unfortunately our cache key would have to be a tuple
// containing the protos passed to this function, and we have no utility for
// hashing protos.  We could write our own hash functions, but they'd silently
// break if we ever added a field to one of the protos.  Perhaps we could hack
// using the binary-encoded proto as the hash key, on the assumption that two
// protos being binary-equal is a sufficient, if not necessary, condition for
// proper equality.  But that would still leave us open to having unnecessary
// cache misses and doing extra work.  Overall, caching doesn't seem worth the
// trouble, but we may want to revisit this if we ever find a model where
// caching would speed up compilation a lot.
StatusOr<AutotuneResult> CudnnConvAlgorithmPicker::PickBestAlgorithm(
    const HloCustomCallInstruction* instr) {
  // TODO(timshen): for now only check fp16. It can be expanded to other types,
  // with some work on the HLO routines.
  const bool cross_check_enabled =
      instr->shape().tuple_shapes(0).element_type() == xla::F16;

  // Don't run this function concurrently on the same GPU.
  //
  // This is a bit of a hack and doesn't protect us against arbitrary concurrent
  // use of a GPU, but it's sufficient to let us compile two HLO modules
  // concurrently and then run them sequentially.
  tensorflow::mutex_lock lock = LockGpu(stream_exec_);

  // Make sure any previous activity on this executor is done. We don't want to
  // interfere with programs that are still running on the GPU.
  if (!stream_exec_->SynchronizeAllActivity()) {
    return InternalError("Failed to synchronize GPU for autotuning.");
  }

  // Create a stream for us to do our work on.
  se::Stream stream{stream_exec_};
  stream.Init();
  const auto device_ordinal = stream_exec_->device_ordinal();

  // allocator either points to this->allocator_ or, if that's null, to a
  // StreamExecutorMemoryAllocator for stream_exec_.
  DeviceMemoryAllocator* allocator;
  optional<StreamExecutorMemoryAllocator> se_allocator;
  if (allocator_ != nullptr) {
    allocator = allocator_;
  } else {
    se_allocator.emplace(stream_exec_->platform(),
                         absl::Span<se::StreamExecutor* const>({stream_exec_}));
    allocator = &*se_allocator;
  }

  const auto initialize_buffer = [&stream, cross_check_enabled](
                                     DeviceMemoryBase buffer) {
    if (cross_check_enabled) {
      // Broadcast a constant to the buffer, instead of zeroing the buffer. A
      // non-zero constant is useful for the cross checking, because zero-inputs
      // may not always reveal the bugs.
      CHECK_EQ(0, (uintptr_t)buffer.opaque() % 4);
      size_t left_over_bytes = buffer.size() % 4;
      CHECK_EQ(0, left_over_bytes % 2);

      constexpr float kBroadcastedConstant = 0.1f;
      static const Eigen::half halfs[2] = {Eigen::half(kBroadcastedConstant),
                                           Eigen::half(kBroadcastedConstant)};
      uint32 bits;
      static_assert(sizeof(bits) == sizeof(halfs), "");
      memcpy(&bits, halfs, sizeof(bits));

      size_t aligned_size = buffer.size() / 4 * 4;
      stream.ThenMemset32(&buffer, bits, aligned_size);

      DeviceMemoryBase left_over(
          static_cast<char*>(buffer.opaque()) + aligned_size, left_over_bytes);
      stream.ThenMemcpy(&left_over, halfs, left_over_bytes);
    } else {
      // Although we don't have evidence this matters, zero out the buffers
      // before autotuning.  It's conceivable that using uninitialized memory as
      // the inputs might affect performance if e.g. the inputs contain
      // denormals, and this is easy enough.
      stream.ThenMemZero(&buffer, buffer.size());
    }
  };

  // Allocate space for the input, filter, and output of the convolution.  We
  // use a ScratchAllocator for this instead of calling allocator_ directly so
  // that our allocations don't leak.
  ScratchAllocator input_output_allocator(device_ordinal, allocator);
  std::vector<se::DeviceMemoryBase> operand_buffers;
  for (const auto* operand : instr->operands()) {
    TF_ASSIGN_OR_RETURN(auto buffer,
                        input_output_allocator.AllocateBytes(
                            &stream, ShapeUtil::ByteSizeOf(operand->shape())));
    initialize_buffer(buffer);
    operand_buffers.push_back(buffer);
  }
  TF_ASSIGN_OR_RETURN(
      auto result_buffer,
      input_output_allocator.AllocateBytes(
          &stream, ShapeUtil::ByteSizeOf(instr->shape().tuple_shapes(0))));
  initialize_buffer(result_buffer);

  TF_ASSIGN_OR_RETURN(auto backend_config,
                      instr->backend_config<CudnnConvBackendConfig>());

  optional<F16BufferComparator> comparator;
  // Use the first algorithm that's supported as reference. There isn't a
  // particular reason to use it, as any algorithm sufficies. It doesn't make
  // this algorithm considered correct, though.
  optional<AlgorithmDesc> first_algorithm;
  TF_ASSIGN_OR_RETURN(CudnnConvKind kind, GetCudnnConvKind(instr));
  std::vector<AutotuneResult> profile_results;
  for (const AlgorithmDesc& alg : GetAlgorithms(kind, stream_exec_)) {
    ScratchAllocator scratch_allocator(device_ordinal, allocator);
    se::dnn::ProfileResult profile_result;
    VLOG(3) << "Trying algorithm " << AlgorithmToString(alg) << " for "
            << instr->ToString();

    // Use assignment instead of brace-list to make GCC 4.9 happy.
    RunConvOptions options;
    options.profile_result = &profile_result;
    options.algo_override = alg;
    Status launch_status =
        RunCudnnConv(instr, absl::MakeSpan(operand_buffers), result_buffer,
                     &scratch_allocator, &stream, options);

    if (!launch_status.ok()) {
      continue;
    }

    if (!profile_result.is_valid()) {
      continue;
    }

    profile_results.emplace_back();
    AutotuneResult& result = profile_results.back();
    result.mutable_conv()->set_algorithm(alg.algo_id());
    result.mutable_conv()->set_tensor_ops_enabled(alg.tensor_ops_enabled());

    int64 scratch_bytes_used = scratch_allocator.TotalAllocatedBytes();
    result.mutable_success()->set_scratch_bytes(scratch_bytes_used);
    *result.mutable_success()->mutable_run_time() =
        tensorflow::proto_utils::ToDurationProto(
            absl::Milliseconds(profile_result.elapsed_time_in_ms()));

    const bool crash_on_checking_failure =
        instr->GetModule()
            ->config()
            .debug_options()
            .xla_gpu_crash_on_verification_failures();

    if (comparator.has_value()) {
      StatusOr<bool> compare_result = comparator->CompareEqual(
          se::DeviceMemory<Eigen::half>(result_buffer));
      if (!compare_result.ok()) {
        LOG(ERROR) << "Unable to compare "
                   << AlgorithmToString(*first_algorithm) << " against "
                   << AlgorithmToString(alg) << " for " << instr->ToString()
                   << ": " << compare_result.status();
        CHECK(!crash_on_checking_failure);
      } else if (!compare_result.ValueOrDie()) {
        LOG(ERROR) << "Results mismatch between different convolution "
                      "algorithms. This is likely a bug in convolution, or "
                      "an excessive loss of precision in convolution. "
                   << instr->ToString() << " for "
                   << AlgorithmToString(*first_algorithm) << " vs "
                   << AlgorithmToString(alg);
        CHECK(!crash_on_checking_failure);
        auto* failure = result.mutable_reference_conv();
        failure->set_algorithm(first_algorithm->algo_id());
        failure->set_tensor_ops_enabled(first_algorithm->tensor_ops_enabled());
      }
    } else if (cross_check_enabled) {
      auto comp = F16BufferComparator::Create(
          se::DeviceMemory<Eigen::half>(result_buffer), compiler_, allocator,
          &stream);
      if (comp.ok()) {
        comparator.emplace(comp.ConsumeValueOrDie());
        first_algorithm.emplace(alg);
      } else {
        LOG(ERROR) << "Fail to initialize buffer comparator: " << comp.status()
                   << ", instruction: " << instr->ToString();
        CHECK(!crash_on_checking_failure);
      }
    }
  }

  // Log the autotuning result.
  {
    tensorflow::AutotuningLog log;
    {
      ConvInstructionLog instr_log;
      *instr_log.mutable_instruction() = instr->ToProto();
      for (const auto* op : instr->operands()) {
        *instr_log.add_operand_shapes() = op->shape().ToProto();
      }
      log.mutable_instr()->PackFrom(instr_log);
    }
    for (const auto& profile : profile_results) {
      *log.add_results() = profile;
    }
    *log.mutable_compute_capability() = GetComputeCapability(stream_exec_);
    *log.mutable_cudnn_version() = GetCudnnVersion(stream_exec_);
    VLOG(2) << "Autotuning result:\n" << log.DebugString();
    tensorflow::Logger::Singleton()->LogProto(log);
  }

  auto* profile_results_end = profile_results.data() + profile_results.size();

  const AutotuneResult* best_result = std::min_element(
      profile_results.data(), profile_results_end,
      [](const AutotuneResult& lhs, const AutotuneResult& rhs) {
        // The successful one should have a smaller key, since we are doing
        // min_element. If they are both unsuccessful, keep the earlier one in
        // the vector by comparing pointers.
        return std::make_tuple(!lhs.has_success(),
                               tensorflow::proto_utils::FromDurationProto(
                                   lhs.success().run_time()),
                               &lhs) <
               std::make_tuple(!rhs.has_success(),
                               tensorflow::proto_utils::FromDurationProto(
                                   rhs.success().run_time()),
                               &rhs);
      });

  if (best_result != profile_results_end && best_result->has_success()) {
    return *best_result;
  }

  return InternalError(
      "All algorithms tried for convolution %s failed.  Falling back to "
      "default algorithm.",
      instr->ToString());
}

StatusOr<bool> CudnnConvAlgorithmPicker::RunOnInstruction(
    HloInstruction* instr) {
  CHECK(IsCustomCallToDnnConvolution(*instr));

  StatusOr<AutotuneResult> best_algo_or =
      PickBestAlgorithm(Cast<HloCustomCallInstruction>(instr));
  if (!best_algo_or.ok()) {
    LOG(ERROR) << best_algo_or.status();
    return false;
  }

  auto best_algo = std::move(best_algo_or).ValueOrDie();
  VLOG(1) << "Setting cudnn conv to use algorithm "
          << best_algo.conv().algorithm() << " and "
          << NumBytesToString(best_algo.success().scratch_bytes())
          << " of scratch memory: " << instr->ToString()
          << " tensor_ops_enabled: " << best_algo.conv().tensor_ops_enabled();

  // Replace instr with a new CustomCall which has the correct algorithm, and
  // whose output shape has the appropriate amount of scratch memory.
  HloComputation* computation = instr->parent();
  Shape new_call_shape = ShapeUtil::MakeTupleShape(
      {instr->shape().tuple_shapes(0),
       ShapeUtil::MakeShape(U8, {best_algo.success().scratch_bytes()})});

  TF_ASSIGN_OR_RETURN(CudnnConvBackendConfig backend_config,
                      instr->backend_config<CudnnConvBackendConfig>());
  backend_config.set_algorithm(best_algo.conv().algorithm());
  backend_config.set_tensor_ops_enabled(best_algo.conv().tensor_ops_enabled());

  HloInstruction* new_call = computation->AddInstruction(
      instr->CloneWithNewOperands(new_call_shape, instr->operands()));

  VLOG(1) << "Replacing convolution " << instr->ToString() << " with "
          << new_call->ToString();

  TF_RETURN_IF_ERROR(new_call->set_backend_config(backend_config));

  // Repackage new_call so it has the same shape as the original call, namely
  // (conv_result, u8[0]).
  HloInstruction* new_tuple =
      computation->AddInstruction(HloInstruction::CreateTuple(
          {computation->AddInstruction(HloInstruction::CreateGetTupleElement(
               new_call_shape.tuple_shapes(0), new_call, 0)),
           computation->AddInstruction(HloInstruction::CreateConstant(
               LiteralUtil::CreateR1<uint8>({})))}));

  TF_RETURN_IF_ERROR(instr->parent()->ReplaceInstruction(instr, new_tuple));
  return true;
}

StatusOr<bool> CudnnConvAlgorithmPicker::RunOnComputation(
    HloComputation* computation) {
  std::vector<HloInstruction*> convs;
  for (auto* instr : computation->instructions()) {
    if (IsCustomCallToDnnConvolution(*instr)) {
      convs.push_back(instr);
    }
  }

  bool changed = false;
  for (auto* instr : convs) {
    TF_ASSIGN_OR_RETURN(bool result, RunOnInstruction(instr));
    changed |= result;
  }
  return changed;
}

StatusOr<bool> CudnnConvAlgorithmPicker::Run(HloModule* module) {
  bool changed = false;
  for (HloComputation* computation : module->MakeNonfusionComputations()) {
    TF_ASSIGN_OR_RETURN(bool result, RunOnComputation(computation));
    changed |= result;
  }
  return changed;
}

}  // namespace gpu
}  // namespace xla
