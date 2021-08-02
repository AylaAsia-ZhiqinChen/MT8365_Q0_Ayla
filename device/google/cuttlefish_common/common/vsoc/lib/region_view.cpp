#include "common/vsoc/lib/region_view.h"

#include <sys/mman.h>

#include "common/libs/glog/logging.h"

namespace {
const uint32_t UADDR_OFFSET_MASK = 0xFFFFFFFC;
const uint32_t UADDR_OFFSET_ROUND_TRIP_FLAG = 1;
}  // namespace

using vsoc::layout::Sides;

vsoc::RegionWorker::RegionWorker(RegionView* region,
                                 std::shared_ptr<RegionControl> control)
    : control_(control),
      region_(region),
      stopping_(false) {}

void vsoc::RegionWorker::start() {
  CHECK(!thread_.joinable());
  thread_ = std::thread(&vsoc::RegionWorker::Work, this);
}

void vsoc::RegionWorker::Work() {
  while (!stopping_) {
    region_->WaitForInterrupt();
    if (stopping_) {
      return;
    }
    region_->ProcessSignalsFromPeer([this](uint32_t offset) {
        control_->SignalSelf(offset);
    });
  }
}

vsoc::RegionWorker::~RegionWorker() {
  stopping_ = true;

  if (thread_.joinable()) {
    region_->InterruptSelf();
    thread_.join();
  }
}

vsoc::RegionView::~RegionView() {
  // region_base_ is borrowed here. It's owned by control_, which is
  // responsible for unmapping the memory
  region_base_ = nullptr;
}

#if defined(CUTTLEFISH_HOST)
bool vsoc::RegionView::Open(const char* name, const char* domain) {
  control_ = vsoc::RegionControl::Open(name, domain);
  if (!control_) {
    return false;
  }
  region_base_ = control_->Map();
  return region_base_ != nullptr;
}
#else
bool vsoc::RegionView::Open(const char* name) {
  control_ = vsoc::RegionControl::Open(name);
  if (!control_) {
    return false;
  }
  region_base_ = control_->Map();
  return region_base_ != nullptr;
}
#endif

// Interrupt our peer, causing it to scan the outgoing_signal_table
bool vsoc::RegionView::MaybeInterruptPeer() {
  if (region_offset_to_pointer<std::atomic<uint32_t>>(
          outgoing_signal_table().interrupt_signalled_offset)
          ->exchange(1)) {
    return false;
  }
  return control_->InterruptPeer();
}

// Wait for an interrupt from our peer
void vsoc::RegionView::WaitForInterrupt() {
  while (1) {
    if (region_offset_to_pointer<std::atomic<uint32_t>>(
            incoming_signal_table().interrupt_signalled_offset)
            ->exchange(0)) {
      return;
    }
    control_->WaitForInterrupt();
  }
}

void vsoc::RegionView::ProcessSignalsFromPeer(
    std::function<void(uint32_t)> signal_handler) {
  const vsoc_signal_table_layout& table = incoming_signal_table();
  const size_t num_offsets = (1 << table.num_nodes_lg2);
  std::atomic<uint32_t>* offsets =
      region_offset_to_pointer<std::atomic<uint32_t>>(
          table.futex_uaddr_table_offset);
  for (size_t i = 0; i < num_offsets; ++i) {
    uint32_t raw_offset = offsets[i].exchange(0);
    if (raw_offset) {
      bool round_trip = raw_offset & UADDR_OFFSET_ROUND_TRIP_FLAG;
      uint32_t offset = raw_offset & UADDR_OFFSET_MASK;
      signal_handler(offset);
      if (round_trip) {
        SendSignalToPeer(
            region_offset_to_pointer<std::atomic<uint32_t>>(offset), false);
      }
    }
  }
}

void vsoc::RegionView::SendSignal(Sides sides_to_signal,
                                  std::atomic<uint32_t>* uaddr) {
  if (sides_to_signal & Sides::Peer) {
    // If we should also be signalling our side set the round trip flag on
    // the futex signal.
    bool round_trip = sides_to_signal & Sides::OurSide;
    SendSignalToPeer(uaddr, round_trip);
    // Return without signaling our waiters to give the other side a chance
    // to run.
    return;
  }
  if (sides_to_signal & Sides::OurSide) {
    control_->SignalSelf(pointer_to_region_offset(uaddr));
  }
}

void vsoc::RegionView::SendSignalToPeer(std::atomic<uint32_t>* uaddr,
                                        bool round_trip) {
  const vsoc_signal_table_layout& table = outgoing_signal_table();
  std::atomic<uint32_t>* offsets =
      region_offset_to_pointer<std::atomic<uint32_t>>(
          table.futex_uaddr_table_offset);
  // maximum index in the node that can hold an offset;
  const size_t max_index = (1 << table.num_nodes_lg2) - 1;
  uint32_t offset = pointer_to_region_offset(uaddr);
  if (offset & ~UADDR_OFFSET_MASK) {
    LOG(FATAL) << "uaddr offset is not naturally aligned " << uaddr;
  }
  // Guess at where this offset should go in the table.
  // Do this before we set the round-trip flag.
  size_t hash = (offset >> 2) & max_index;
  if (round_trip) {
    offset |= UADDR_OFFSET_ROUND_TRIP_FLAG;
  }
  while (1) {
    uint32_t expected = 0;
    if (offsets[hash].compare_exchange_strong(expected, offset)) {
      // We stored the offset. Send the interrupt.
      this->MaybeInterruptPeer();
      break;
    }
    // We didn't store, but the value was already in the table with our flag.
    // Return without interrupting.
    if (expected == offset) {
      return;
    }
    // Hash collision. Try again in a different node
    if ((expected & UADDR_OFFSET_MASK) != (offset & UADDR_OFFSET_MASK)) {
      hash = (hash + 1) & max_index;
      continue;
    }
    // Our offset was in the bucket, but the flags didn't match.
    // We're done if the value in the node had the round trip flag set.
    if (expected & UADDR_OFFSET_ROUND_TRIP_FLAG) {
      return;
    }
    // We wanted the round trip flag, but the value in the bucket didn't set it.
    // Do a second swap to try to set it.
    if (offsets[hash].compare_exchange_strong(expected, offset)) {
      // It worked. We're done.
      return;
    }
    if (expected == offset) {
      // expected was the offset without the flag. After the swap it has the
      // the flag. This means that some other thread set the flag, so
      // we're done.
      return;
    }
    // Something about the offset changed. We need to go around again, because:
    //   our peer processed the old entry
    //   another thread may have stolen the node while we were distracted
  }
}

std::unique_ptr<vsoc::RegionWorker> vsoc::RegionView::StartWorker() {
    std::unique_ptr<vsoc::RegionWorker> worker(
            new vsoc::RegionWorker(this /* region */, control_));

    worker->start();
    return worker;
}

int vsoc::RegionView::WaitForSignal(std::atomic<uint32_t>* uaddr,
                                     uint32_t expected_value) {
  return control_->WaitForSignal(pointer_to_region_offset(uaddr),
                                 expected_value);
}
