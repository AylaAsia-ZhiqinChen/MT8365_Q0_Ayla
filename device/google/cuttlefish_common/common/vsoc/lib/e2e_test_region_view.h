#pragma once
/*
 * Copyright (C) 2016 The Android Open Source Project
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

#include <memory>
#include <functional>

#include "common/vsoc/lib/typed_region_view.h"
#include "common/vsoc/shm/e2e_test_region_layout.h"

namespace vsoc {
template <typename Layout>
class E2ERegionView : public vsoc::TypedRegionView<
                      E2ERegionView<Layout>,
                      Layout> {
 public:

  uint32_t read_guest_self_register() const {
    return this->data().guest_self_register;
  }

  void write_guest_self_register(uint32_t val) {
    this->data()->guest_self_register = val;
  }

  void signal_guest_self_register() {
    this->SendSignal(layout::Sides::OurSide,
                     &this->data()->guest_self_register);
  }

  int wait_guest_self_register(uint32_t expected_value) {
    return this->WaitForSignal(
        &this->data()->guest_self_register, expected_value);
  }

  void signal_guest_to_host_register() {
    this->SendSignal(layout::Sides::OurSide,
                     &this->data()->guest_to_host_signal);
  }

  uint32_t guest_to_host_signal_offset() const {
    return this->pointer_to_region_offset(&this->data().guest_to_host_signal);
  }

  uint32_t host_to_guest_signal_offset() const {
    return this->pointer_to_region_offset(&this->data().host_to_guest_signal);
  }

  const char* guest_string(size_t index) const {
    return const_cast<const char*>(this->data().data[index].guest_writable);
  }

  const char* host_string(size_t index) const {
    return const_cast<const char*>(this->data().data[index].host_writable);
  }

  bool set_guest_string(size_t index, const char* value) {
    strcpy(const_cast<char*>(this->data()->data[index].guest_writable),
           value);
    return true;
  }

  bool set_host_string(size_t index, const char* value) {
    strcpy(const_cast<char*>(this->data()->data[index].host_writable),
           value);
    return true;
  }

  size_t string_size() const {
    return Layout::NumFillRecords(this->control_->region_data_size());
  }

  void guest_status(vsoc::layout::e2e_test::E2ETestStage stage) {
    this->data()->guest_status.set_value(stage);
  }

  void host_status(vsoc::layout::e2e_test::E2ETestStage stage) {
    this->data()->host_status.set_value(stage);
  }
};

using E2EPrimaryRegionView =
  vsoc::E2ERegionView<layout::e2e_test::E2EPrimaryTestRegionLayout>;

using E2ESecondaryRegionView =
  vsoc::E2ERegionView<layout::e2e_test::E2ESecondaryTestRegionLayout>;

using E2EUnfindableRegionView =
  vsoc::E2ERegionView<layout::e2e_test::E2EUnfindableRegionLayout>;

}  // namespace vsoc
