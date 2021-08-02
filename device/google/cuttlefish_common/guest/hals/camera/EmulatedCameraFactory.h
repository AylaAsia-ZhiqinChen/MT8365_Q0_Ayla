/*
 * Copyright (C) 2011 The Android Open Source Project
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

#ifndef HW_EMULATOR_CAMERA_EMULATED_CAMERA_FACTORY_H
#define HW_EMULATOR_CAMERA_EMULATED_CAMERA_FACTORY_H

#include <utils/RefBase.h>

#include <utils/Vector.h>
#include "CameraConfiguration.h"
#include "EmulatedBaseCamera.h"
#include "common/libs/threads/cuttlefish_thread.h"
#include "guest/libs/platform_support/api_level_fixes.h"

namespace android {

class EmulatedCameraHotplugThread;

/*
 * Contains declaration of a class EmulatedCameraFactory that manages cameras
 * available for the emulation. A global instance of this class is statically
 * instantiated and initialized when camera emulation HAL is loaded.
 */

/* Class EmulatedCameraFactoryManages cameras available for the emulation.
 *
 * When the global static instance of this class is created on the module load,
 * it enumerates cameras available for the emulation by connecting to the
 * emulator's 'camera' service. For every camera found out there it creates an
 * instance of an appropriate class, and stores it an in array of emulated
 * cameras. In addition to the cameras reported by the emulator, a fake camera
 * emulator is always created, so there is always at least one camera that is
 * available.
 *
 * Instance of this class is also used as the entry point for the camera HAL
 * API, including:
 *  - hw_module_methods_t::open entry point
 *  - camera_module_t::get_number_of_cameras entry point
 *  - camera_module_t::get_camera_info entry point
 *
 */
class EmulatedCameraFactory {
 public:
  /* Constructs EmulatedCameraFactory instance.
   * In this constructor the factory will create and initialize a list of
   * emulated cameras. All errors that occur on this constructor are reported
   * via mConstructedOK data member of this class.
   */
  EmulatedCameraFactory();

  /* Destructs EmulatedCameraFactory instance. */
  ~EmulatedCameraFactory();

  /****************************************************************************
   * Camera HAL API handlers.
   ***************************************************************************/

 public:
  /* Returns a (singleton) instance of the EmulatedCameraFactory.
   */
  static EmulatedCameraFactory& Instance();

  /* Opens (connects to) a camera device.
   * This method is called in response to hw_module_methods_t::open callback.
   */
  int cameraDeviceOpen(int camera_id, hw_device_t** device);

  /* Gets emulated camera information.
   * This method is called in response to camera_module_t::get_camera_info
   * callback.
   */
  int getCameraInfo(int camera_id, struct camera_info* info);

#if VSOC_PLATFORM_SDK_AFTER(J_MR2)
  /* Sets emulated camera callbacks.
   * This method is called in response to camera_module_t::set_callbacks
   * callback.
   */
  int setCallbacks(const camera_module_callbacks_t* callbacks);

  /* Fill in vendor tags for the module
   * This method is called in response to camera_module_t::get_vendor_tag_ops
   * callback.
   */
  void getVendorTagOps(vendor_tag_ops_t* ops);
#endif

  int setTorchMode(const char* camera_id, bool enabled);

  /****************************************************************************
   * Camera HAL API callbacks.
   ***************************************************************************/

 public:
  /* camera_module_t::get_number_of_cameras callback entry point. */
  static int get_number_of_cameras(void);

  /* camera_module_t::get_camera_info callback entry point. */
  static int get_camera_info(int camera_id, struct camera_info* info);

#if VSOC_PLATFORM_SDK_AFTER(J_MR2)
  /* camera_module_t::set_callbacks callback entry point. */
  static int set_callbacks(const camera_module_callbacks_t* callbacks);

  /* camera_module_t::get_vendor_tag_ops callback entry point */
  static void get_vendor_tag_ops(vendor_tag_ops_t* ops);
#endif

  /* camera_module_t::open_legacy callback entry point */
  static int open_legacy(const struct hw_module_t* module, const char* id,
                         uint32_t halVersion, struct hw_device_t** device);

  static int set_torch_mode(const char* camera_id, bool enabled);

 private:
  /* hw_module_methods_t::open callback entry point. */
  static int device_open(const hw_module_t* module, const char* name,
                         hw_device_t** device);

  /****************************************************************************
   * Public API.
   ***************************************************************************/

 public:
  /* Gets fake camera orientation. */
  int getFakeCameraOrientation() {
    /* TODO: Have a boot property that controls that. */
    return 90;
  }

  /* Gets number of emulated cameras.
   */
  inline size_t getEmulatedCameraNum() const {
    return mCameraDefinitions.size();
  }

  void onStatusChanged(int cameraId, int newStatus);

  void onTorchModeStatusChanged(int cameraId, int newStatus);

  /****************************************************************************
   * Private API
   ***************************************************************************/

 private:
  /* Create new or return existing fake camera based on camera definition
   * found in mCameraDefinitions.
   * Returns NULL if cameraId is not valid (= not a valid index of
   * mCameraDefinitions)
   */
  EmulatedBaseCamera* getOrCreateFakeCamera(size_t cameraId);

  /****************************************************************************
   * Data members.
   ***************************************************************************/

 private:
  /* Array of cameras available for the emulation. */
  Vector<EmulatedBaseCamera*> mEmulatedCameras;

  /* Guards access to mEmulatedCameras. */
  cvd::Mutex mEmulatedCamerasMutex;

#if VSOC_PLATFORM_SDK_AFTER(J_MR2)
  /* Camera callbacks (for status changing) */
  const camera_module_callbacks_t* mCallbacks;

  /* Hotplug thread (to call onStatusChanged) */
  sp<EmulatedCameraHotplugThread> mHotplugThread;
#endif

  /* Back- and front camera properties accessed from the vsoc device. */
  cvd::CameraConfiguration mCameraConfiguration;
  Vector<cvd::CameraDefinition> mCameraDefinitions;

 public:
  /* Contains device open entry point, as required by HAL API. */
  static struct hw_module_methods_t mCameraModuleMethods;
};

}; /* namespace android */

#endif /* HW_EMULATOR_CAMERA_EMULATED_CAMERA_FACTORY_H */
