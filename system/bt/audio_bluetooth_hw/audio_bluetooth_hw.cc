/*
 * Copyright 2019 The Android Open Source Project
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

#define LOG_TAG "BTAudioHw"

#include <android-base/logging.h>
#include <errno.h>
#include <hardware/audio.h>
#include <hardware/hardware.h>
#include <log/log.h>
#include <malloc.h>
#include <string.h>
#include <system/audio.h>

#include "stream_apis.h"
#include "utils.h"

static int adev_set_parameters(struct audio_hw_device* dev,
                               const char* kvpairs) {
  LOG(VERBOSE) << __func__ << ": kevpairs=[" << kvpairs << "]";
  int retval = 0;
  auto* bluetooth_dev = reinterpret_cast<bluetooth_audio_dev*>(dev);
  std::unique_lock<std::mutex> lock(*bluetooth_dev->mutex_);
  if (bluetooth_dev->output == nullptr) return retval;
  auto* out = bluetooth_dev->output;
  retval =
    out->stream_out_.common.set_parameters((struct audio_stream*)out, kvpairs);
  return retval;
}

static char* adev_get_parameters(const struct audio_hw_device* dev,
                                 const char* keys) {
  LOG(VERBOSE) << __func__ << ": keys=[" << keys << "]";
  return strdup("");
}

static int adev_init_check(const struct audio_hw_device* dev) { return 0; }

static int adev_set_voice_volume(struct audio_hw_device* dev, float volume) {
  LOG(VERBOSE) << __func__ << ": volume=" << volume;
  return -ENOSYS;
}

static int adev_set_master_volume(struct audio_hw_device* dev, float volume) {
  LOG(VERBOSE) << __func__ << ": volume=" << volume;
  return -ENOSYS;
}

static int adev_get_master_volume(struct audio_hw_device* dev, float* volume) {
  return -ENOSYS;
}

static int adev_set_master_mute(struct audio_hw_device* dev, bool muted) {
  LOG(VERBOSE) << __func__ << ": mute=" << muted;
  return -ENOSYS;
}

static int adev_get_master_mute(struct audio_hw_device* dev, bool* muted) {
  return -ENOSYS;
}

static int adev_set_mode(struct audio_hw_device* dev, audio_mode_t mode) {
  LOG(VERBOSE) << __func__ << ": mode=" << mode;
  return 0;
}

static int adev_set_mic_mute(struct audio_hw_device* dev, bool state) {
  LOG(VERBOSE) << __func__ << ": state=" << state;
  return -ENOSYS;
}

static int adev_get_mic_mute(const struct audio_hw_device* dev, bool* state) {
  return -ENOSYS;
}

static int adev_dump(const audio_hw_device_t* device, int fd) { return 0; }

static int adev_close(hw_device_t* device) {
  auto* bluetooth_dev = reinterpret_cast<bluetooth_audio_dev*>(device);
  delete bluetooth_dev->mutex_;
  bluetooth_dev->mutex_ = nullptr;
  free(device);
  return 0;
}

static int adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device) {
  LOG(VERBOSE) << __func__ << ": name=[" << name << "]";
  if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0) return -EINVAL;

  struct bluetooth_audio_dev* adev =
      (struct bluetooth_audio_dev*)calloc(1, sizeof(struct bluetooth_audio_dev));
  if (!adev) return -ENOMEM;

  adev->mutex_ = new std::mutex;
  adev->device.common.tag = HARDWARE_DEVICE_TAG;
  adev->device.common.version = AUDIO_DEVICE_API_VERSION_2_0;
  adev->device.common.module = (struct hw_module_t*)module;
  adev->device.common.close = adev_close;

  adev->device.init_check = adev_init_check;
  adev->device.set_voice_volume = adev_set_voice_volume;
  adev->device.set_master_volume = adev_set_master_volume;
  adev->device.get_master_volume = adev_get_master_volume;
  adev->device.set_mode = adev_set_mode;
  adev->device.set_mic_mute = adev_set_mic_mute;
  adev->device.get_mic_mute = adev_get_mic_mute;
  adev->device.set_parameters = adev_set_parameters;
  adev->device.get_parameters = adev_get_parameters;
  adev->device.get_input_buffer_size = adev_get_input_buffer_size;
  adev->device.open_output_stream = adev_open_output_stream;
  adev->device.close_output_stream = adev_close_output_stream;
  adev->device.open_input_stream = adev_open_input_stream;
  adev->device.close_input_stream = adev_close_input_stream;
  adev->device.dump = adev_dump;
  adev->device.set_master_mute = adev_set_master_mute;
  adev->device.get_master_mute = adev_get_master_mute;

  *device = &adev->device.common;
  adev->output = nullptr;
  return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
    .common =
        {
            .tag = HARDWARE_MODULE_TAG,
            .module_api_version = AUDIO_MODULE_API_VERSION_0_1,
            .hal_api_version = HARDWARE_HAL_API_VERSION,
            .id = AUDIO_HARDWARE_MODULE_ID,
            .name = "Bluetooth Audio HW HAL",
            .author = "The Android Open Source Project",
            .methods = &hal_module_methods,
        },
};
