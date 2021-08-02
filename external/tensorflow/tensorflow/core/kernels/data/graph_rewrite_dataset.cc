/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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
#include "tensorflow/core/kernels/data/graph_rewrite_dataset.h"

#include "tensorflow/core/framework/dataset.h"
#include "tensorflow/core/kernels/data/dataset_utils.h"
#include "tensorflow/core/protobuf/meta_graph.pb.h"
#include "tensorflow/core/protobuf/rewriter_config.pb.h"

namespace tensorflow {
namespace data {

GraphRewriteDataset::~GraphRewriteDataset() {
  input_->Unref();
  if (optimized_input_) {
    optimized_input_->Unref();
  }
}

Status GraphRewriteDataset::Optimize(OpKernelContext* ctx) {
  GraphDefBuilder b;
  DatasetGraphDefBuilder db(&b);
  Node* input_node = nullptr;
  SerializationContext::Params params;
  std::vector<std::pair<string, Tensor>> input_list;
  params.flib_def = ctx->function_library()->GetFunctionLibraryDefinition();
  params.input_list = &input_list;
  params.optimization_only = true;
  SerializationContext serialization_ctx(params);
  TF_RETURN_IF_ERROR(
      db.AddInputDataset(&serialization_ctx, input_, &input_node));
  string output_node = input_node->name();

  GraphDef graph_def;
  TF_RETURN_IF_ERROR(b.ToGraphDef(&graph_def));
  VLOG(3) << "Before optimization: " << graph_def.DebugString();

  TF_RETURN_IF_ERROR(ApplyOptimizations(ctx, &graph_def, &output_node));
  VLOG(3) << "After optimization: " << graph_def.DebugString();

  // Instantiate the optimized input pipeline by running the optimized graph
  // using the optimized function library.
  TF_RETURN_IF_ERROR(ctx->function_library()->Clone(&flib_def_, &pflr_, &lib_));

  // Create a FunctionHandleCache.
  function_handle_cache_ = absl::make_unique<FunctionHandleCache>(lib_);

  // Some functions may have been modified without having their names
  // changed (for example, nested dataset graphs from FlatMap or
  // Interleave).
  TF_RETURN_IF_ERROR(
      AddToFunctionLibrary(flib_def_.get(), graph_def.library()));

  Graph graph(OpRegistry::Global());
  TF_RETURN_IF_ERROR(ImportGraphDef({}, graph_def, &graph, nullptr));
  std::vector<Tensor> outputs;
  GraphRunner graph_runner(ctx->function_library()->device());

  TF_RETURN_IF_ERROR(
      graph_runner.Run(&graph, lib_, input_list, {output_node}, &outputs));
  TF_RETURN_IF_ERROR(
      GetDatasetFromVariantTensor(outputs[0], &optimized_input_));
  optimized_input_->Ref();
  return Status::OK();
}

Status GraphRewriteDataset::AsGraphDefInternal(SerializationContext* ctx,
                                               DatasetGraphDefBuilder* b,
                                               Node** output) const {
  SerializationContext::Params params;
  // The optimized input needs access to the newly optimized functions when
  // it is serialized. Here, we use the optimized function library for
  // serialization, which is the union of the function library from the
  // OpKernelContext at dataset creation time and newly optimized functions.
  // This includes all functions that optimized_input_ may use.
  params.flib_def = flib_def_.get();
  params.input_list = ctx->input_list();
  params.optimization_only = ctx->optimization_only();
  SerializationContext optimized_ctx(params);

  // We only serialize the optimized dataset to avoid re-running
  // optimizations when the input pipeline is restored from a checkpoint.
  TF_RETURN_IF_ERROR(
      b->AddInputDataset(&optimized_ctx, optimized_input_, output));
  return Status::OK();
}

namespace {
void AddFakeSinks(FunctionDef* function_def) {
  int counter = 0;
  for (const auto& output : function_def->signature().output_arg()) {
    NodeDef* node = function_def->add_node_def();
    tensorflow::grappler::function_utils::SetUniqueFunctionNodeName(
        strings::StrCat("FakeSink", counter++), function_def, node);
    node->set_op("Identity");
    node->add_input(function_def->ret().at(output.name()));
    (*node->mutable_attr())["T"].set_type(output.type());

    (*function_def->mutable_ret())[output.name()] =
        strings::StrCat(node->name(), ":output:0");
  }
}

void RemoveFakeSinks(FunctionDef* function_def) {
  // Map from identity node names to their input tensor strings
  std::map<string, string> identity_map;
  for (const auto& node : function_def->node_def()) {
    if (node.op() == "Identity" && node.input_size() == 1) {
      identity_map[node.name()] = node.input(0);
    }
  }
  for (const auto& output_arg : function_def->signature().output_arg()) {
    const string& tensor = function_def->ret().at(output_arg.name());
    const string& output_node = tensor.substr(0, tensor.find(':'));
    if (identity_map.find(output_node) != identity_map.end()) {
      (*function_def->mutable_ret())[output_arg.name()] =
          identity_map.at(output_node);
    }
  }
}
}  // anonymous namespace

Status GraphRewriteDataset::ApplyOptimizations(OpKernelContext* ctx,
                                               GraphDef* graph_def,
                                               string* output_node) {
  // Add an identity node as the fetch node, otherwise we might get
  // 'placeholder is both fed and fetched' errors in some cases when using
  // input list with placeholder dataset nodes.
  NodeDef* node = graph_def->mutable_node()->Add();
  tensorflow::grappler::graph_utils::SetUniqueGraphNodeName("Sink", graph_def,
                                                            node);
  node->set_op("Identity");
  node->add_input(*output_node);
  (*node->mutable_attr())["T"].set_type(DT_VARIANT);
  *output_node = node->name();

  // Add fake sink node to graph and functions to allow rewriting the actual
  // sink nodes.
  // TODO(b/118820916): When MetaOptimizer adds provisions for function
  // retvals to be optimizable, we will no longer need this.
  for (auto& function_def : *graph_def->mutable_library()->mutable_function()) {
    AddFakeSinks(&function_def);
  }

  // Create metagraph.
  MetaGraphDef meta_graph_def;
  (*meta_graph_def.mutable_graph_def()) = *graph_def;

  // Grappler determines fetch ops from collection 'train_op'.
  CollectionDef collection_def;
  auto node_list = collection_def.mutable_node_list();
  node_list->add_value(*output_node);
  (*meta_graph_def.mutable_collection_def())["train_op"] = collection_def;

  // Create Grappler item.
  tensorflow::grappler::ItemConfig item_config;
  item_config.apply_optimizations = true;
  std::unique_ptr<tensorflow::grappler::GrapplerItem> grappler_item =
      tensorflow::grappler::GrapplerItemFromMetaGraphDef(
          "graph", meta_graph_def, item_config);
  grappler_item->optimization_options().optimize_function_library =
      ShouldOptimizeFunctions();
  std::unordered_map<string, tensorflow::DeviceProperties> device_map;
  tensorflow::grappler::VirtualCluster cluster(device_map);

  // Run data optimizer using grappler's meta optimizer.
  tensorflow::ConfigProto config;
  *config.mutable_graph_options()->mutable_rewrite_options() =
      CreateGrapplerRewriteConfig();
  TF_RETURN_IF_ERROR(tensorflow::grappler::RunMetaOptimizer(
      *grappler_item, config, ctx->device(), &cluster, graph_def));

  // Remove fake sinks after optimizations are done.
  // TODO(b/118820916): When MetaOptimizer adds provisions for function
  // retvals to be optimizable, we will no longer need this.
  for (auto& function_def : *graph_def->mutable_library()->mutable_function()) {
    RemoveFakeSinks(&function_def);
  }

  return Status::OK();
}

class GraphRewriteDataset::Iterator
    : public DatasetIterator<GraphRewriteDataset> {
 public:
  explicit Iterator(const Params& params)
      : DatasetIterator<GraphRewriteDataset>(params) {}

  Status Initialize(IteratorContext* ctx) override {
    IteratorContext::Params params(ctx);
    params.lib = dataset()->lib_;
    params.function_handle_cache = dataset()->function_handle_cache_.get();
    return dataset()->optimized_input_->MakeIterator(
        IteratorContext(std::move(params)), prefix(), &input_impl_);
  }

  Status GetNextInternal(IteratorContext* ctx, std::vector<Tensor>* out_tensors,
                         bool* end_of_sequence) override {
    IteratorContext::Params params(ctx);
    params.lib = dataset()->lib_;
    params.function_handle_cache = dataset()->function_handle_cache_.get();
    return input_impl_->GetNext(IteratorContext(std::move(params)), out_tensors,
                                end_of_sequence);
  }

 protected:
  std::shared_ptr<model::Node> CreateNode(
      IteratorContext* ctx, model::Node::Args args) const override {
    return model::MakeKnownRatioNode(std::move(args),
                                     /*ratio=*/1);
  }

  Status SaveInternal(IteratorStateWriter* writer) override {
    TF_RETURN_IF_ERROR(SaveInput(writer, input_impl_));
    return Status::OK();
  }

  Status RestoreInternal(IteratorContext* ctx,
                         IteratorStateReader* reader) override {
    TF_RETURN_IF_ERROR(RestoreInput(ctx, reader, input_impl_));
    return Status::OK();
  }

 private:
  std::unique_ptr<IteratorBase> input_impl_;
};

std::unique_ptr<IteratorBase> GraphRewriteDataset::MakeIteratorInternal(
    const string& prefix) const {
  // We do not add a token for this dataset to the prefix. The
  // prefix is used to identify checkpoint elements and since this
  // dataset is excluded from the checkpoint, adding a token
  // here would result in invalid checkpoint identifiers.
  return absl::make_unique<Iterator>(Iterator::Params{this, prefix});
}

}  // namespace data
}  // namespace tensorflow
