#pragma once

/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sys/mman.h>
#include <stdint.h>
#include <memory>
#include "uapi/vsoc_shm.h"

namespace vsoc {

/**
 * Base class for side-specific utility functions that work on regions.
 * The methods in this class do not assume that the region is mapped in memory.
 * This makes is appropriate for ManagedRegions and certain low-level tests
 * of VSoC shared memory. Most other users will want to use TypedRegions with
 * a defined RegionLayout.
 *
 * This class is not directly instantiable because it must be specialized with
 * additional fields for the host and guest.
 */
class RegionControl {
 public:
  virtual ~RegionControl() {
    if (region_base_) {
      munmap(region_base_, region_size());
    }
    region_base_ = nullptr;
  }

#if defined(CUTTLEFISH_HOST)
  static std::shared_ptr<RegionControl> Open(const char* region_name,
                                             const char* domain);
#else
  static std::shared_ptr<RegionControl> Open(const char* region_name);
#endif

  const vsoc_device_region& region_desc() const { return region_desc_; }

  // Returns the size of the entire region, including the signal tables.
  uint32_t region_size() const {
    return region_desc_.region_end_offset - region_desc_.region_begin_offset;
  }

  // Returns the size of the region that is usable for region-specific data.
  uint32_t region_data_size() const {
    return region_size() - region_desc_.offset_of_region_data;
  }

  // Creates a FdScopedPermission. Returns the file descriptor or -1 on
  // failure. FdScopedPermission is not supported on the host, so -1 is
  // always returned there.
  virtual int CreateFdScopedPermission(const char* managed_region_name,
                                       uint32_t owner_offset,
                                       uint32_t owned_value,
                                       uint32_t begin_offset,
                                       uint32_t end_offset) = 0;

  // Interrupt our peer, causing it to scan the outgoing_signal_table
  virtual bool InterruptPeer() = 0;

  // Wake the local signal table scanner. Primarily used during shutdown
  virtual void InterruptSelf() = 0;

  // Maps the entire region at an address, returning a pointer to the mapping
  virtual void* Map() = 0;

  // Wait for an interrupt from our peer
  virtual void WaitForInterrupt() = 0;

  // Signals local waiters at the given region offset.
  // Defined only on the guest.
  // Return value is negative on error.
  virtual int SignalSelf(uint32_t offset) = 0;

  // Waits for a signal at the given region offset.
  // Defined only on the guest.
  // Return value is negative on error. The number of false wakes is returned
  // on success.
  virtual int WaitForSignal(uint32_t offset, uint32_t expected_value) = 0;

  template <typename T>
  T* region_offset_to_pointer(uint32_t offset) {
    if (offset > region_size()) {
      LOG(FATAL) << __FUNCTION__ << ": " << offset << " not in region @"
                 << region_base_;
    }
    return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(region_base_) +
                                offset);
  }

 protected:
  RegionControl() {}
  void* region_base_{};
  vsoc_device_region region_desc_{};
};
}  // namespace vsoc
