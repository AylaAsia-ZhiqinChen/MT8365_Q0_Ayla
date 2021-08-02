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

// Memory layout for locks of all types.

// The vsoc::layout namespace indicates that these are shared memory structure
// definitions. The #include's given above are strictly limited, as are the
// types that can be referenced below.

// For _mm_pause()
#if defined(__SSE2__)
#include <x86intrin.h>
#define _pause() _mm_pause()
#elif defined(__arm__) || defined(__aarch64__)
#include <arm_acle.h>
#define _pause() __yield()
#endif

#include <atomic>
#include <cstdint>

#include "common/vsoc/shm/base.h"

// Host userspace, guest userspace, and the guest kernel must all agree on
// the relationship between std::atomic and atomic_t. That's hard to do without
// examining assembly, and we can't really examing atomic_t outside of the
// kernel tree, but we can at least assert that the host and the guest
// agree on a size.
static_assert(sizeof(std::atomic<uint32_t>) == 4, "std::atomic size mismatch");

namespace vsoc {

class RegionView;

namespace layout {

/**
 * Lock that causes threads to busy loop rather than sleeping.
 * This lock should never be used when the amount of work in the critical
 * section cannot be bounded.
 */
class SpinLock {
 public:
  static constexpr size_t layout_size = 4;

  /**
   * Acquire the spinlock on the queue. This will effectively block all
   * readers and writers.
   */
  void Lock() {
    while (1) {
      uint32_t expected = 0;
      if (lock_.compare_exchange_strong(expected, Sides::OurSide)) {
        return;
      }
      _pause();
    }
  }

  /**
   * Drop the lock iff it is currently held by this side. Used by
   * recovery code that cleans up regions in the event of a reboot
   * (guest side) or a service restart (host side).
   *
   * The caller must ensure that there are no other threads on its
   * side (e.g. guest/host) are using the window.
   */
  bool Recover() {
    uint32_t expected = Sides::OurSide;
    return lock_.compare_exchange_strong(expected, 0);
  }

  /**
   * Release the spinlock.
   */
  void Unlock() {
    lock_ = 0;
  }

 protected:
  std::atomic<uint32_t> lock_;
};
ASSERT_SHM_COMPATIBLE(SpinLock);

/**
 * This is a generic synchronization primitive that provides space for the
 * owner of the lock to write platform-specific information.
 */
class WaitingLockBase {
 public:
  static constexpr size_t layout_size = 40;

 protected:
  // Common code to handle locking
  // Must be called with the kernel's thread id
  // Returns true if the lock was acquired. In this case the value in
  // expected_vlaue is undefined.
  // Returns false if locking failed. The value discovered in the lock word
  // is returned in expected_value, and should probably be used in a conditional
  // sleep.
  bool TryLock(uint32_t tid, uint32_t* expected_value);

  // Common code to handle unlocking.
  // Must be called with the kernel's thread id
  // Returns sides that should be signalled or 0
  Sides UnlockCommon(uint32_t tid);

  // Common code to recover single-sided locks.
  bool RecoverSingleSided();

  // Non-zero values in this word indicate that the lock is in use.
  // This is 32 bits for compatibility with futex()
  std::atomic<uint32_t> lock_uint32_;

  // Pad so we line up with glib's pthread_mutex_t and can share the same queue.
  // These fields may be redefined at any point in the future. They should not
  // be used.
 private:
// These fields are known to be unused and are provided for compatibility
// with glibc's locks.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
  uint32_t reserved_1_;
  char reserved_2_[16];
  // Provide scratch space for the owner of the lock. The content of this space
  // is undefined when the lock is acquired. The owner may write to and read
  // from it while it holds the lock, but must relinquish control before
  // releasing the lock.
  //
  // This is intended to support Linux robust futexes. See the documentation
  // in the kernel tree:
  //   Documentation/robust-futex-ABI.txt
 public:
  int64_t owner_scratch_[2];
#pragma clang diagnostic pop
};
ASSERT_SHM_COMPATIBLE(WaitingLockBase);

/**
 * GuestLocks can be acquired and released only on the guest. They reside
 * in the shared memory window because mutiple guest processes may need
 * to coordinate activities in certain shared memory regions.
 *
 * Representing this as a concrete type allows for some optimizations when
 * signalling on the lock.
 */
class GuestLock : public WaitingLockBase {
 public:
  static constexpr size_t layout_size = WaitingLockBase::layout_size;

#ifndef CUTTLEFISH_HOST
  void Lock();
  void Unlock();
  /**
   * Drop the lock iff it is currently held. Used by
   * recovery code that cleans up regions in the event of a reboot.
   *
   * The caller must ensure that there are no other threads on its
   * side (e.g. guest/host) are using the window.
   */
  bool Recover();
#endif
};
ASSERT_SHM_COMPATIBLE(GuestLock);

/**
 * HostLocks can be acquired and released only on the host. They reside
 * in the shared memory window because mutiple host processes may need
 * to coordinate activities in certain shared memory regions.
 *
 * Representing this as a concrete type allows for some optimizations when
 * signalling on the lock.
 */
class HostLock : public WaitingLockBase {
 public:
  static constexpr size_t layout_size = WaitingLockBase::layout_size;

#ifdef CUTTLEFISH_HOST
  void Lock();
  void Unlock();
  /**
   * Drop the lock iff it is currently held. Used by
   * recovery code that cleans up regions in the event of a daemon
   * restart.
   *
   * The caller must ensure that there are no other threads on its
   * side (e.g. guest/host) are using the window.
   */
  bool Recover();
#endif
};
ASSERT_SHM_COMPATIBLE(HostLock);

/**
 * GuestAndHostLocks can be acquired and released on either side of the
 * shared memory window. The locks attempt to enforce fairness by using
 * a round-trip signal:
 *
 *   When a guest releases a lock this code sends a signal to wake the host,
 *   but not other guest waiters.
 *
 *   The wake handler on the host wakes up and local waiters and then reposts
 *   the signal to the guest.
 *
 *   When the guest receives the signal from the host it then wakes ups
 *   any waiters.
 *
 * A similar scenario applies when the host releases a lock with guest waiters.
 *
 * Signalling across the shared memory window twice has non-trivial cost.
 * There are some optimizations in the code to prevent the full round-trip
 * if the process releasing the lock can confirm that there are no waiters on
 * the other side.
 *
 * Representing this as a concrete type allows for some optimizations when
 * signalling on the lock.
 */
class GuestAndHostLock : public WaitingLockBase {
 public:
  static constexpr size_t layout_size = WaitingLockBase::layout_size;

  void Lock(RegionView*);
  void Unlock(RegionView*);
  /**
   * Drop the lock iff it is currently held by this side. Used by
   * recovery code that cleans up regions in the event of a reboot
   * (guest side) or a service restart (host side).
   *
   * The caller must ensure that there are no other threads on its
   * side (e.g. guest/host) are using the window.
   */
  bool Recover(RegionView*);
};
ASSERT_SHM_COMPATIBLE(GuestAndHostLock);

}  // namespace layout
}  // namespace vsoc
