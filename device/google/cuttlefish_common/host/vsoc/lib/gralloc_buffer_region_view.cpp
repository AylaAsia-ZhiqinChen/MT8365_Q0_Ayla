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

#include "host/vsoc/lib/gralloc_buffer_region_view.h"

#include <memory>
#include <mutex>
#include "glog/logging.h"

using vsoc::gralloc::GrallocBufferRegionView;

uint8_t* GrallocBufferRegionView::OffsetToBufferPtr(uint32_t offset) {
  if (offset <= control_->region_desc().offset_of_region_data ||
      offset >= control_->region_size()) {
    LOG(FATAL)
        << "Attempted to access a gralloc buffer outside region data, offset: "
        << offset;
    return nullptr;
  }
  return region_offset_to_pointer<uint8_t>(offset);
}
