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

#include "chre/core/sensor_request_manager.h"

#include "chre_api/chre/version.h"
#include "chre/core/event_loop_manager.h"
#include "chre/platform/fatal_error.h"
#include "chre/util/system/debug_dump.h"

namespace chre {
namespace {

bool isSensorRequestValid(const Sensor& sensor,
                          const SensorRequest& sensorRequest) {
  bool isRequestContinuous = sensorModeIsContinuous(
      sensorRequest.getMode());
  bool isRequestOneShot = sensorModeIsOneShot(sensorRequest.getMode());
  uint64_t requestedInterval = sensorRequest.getInterval().toRawNanoseconds();
  SensorType sensorType = sensor.getSensorType();

  bool success = true;
  if (requestedInterval < sensor.getMinInterval()) {
    success = false;
    LOGE("Requested interval %" PRIu64 " < sensor's minInterval %" PRIu64,
         requestedInterval, sensor.getMinInterval());
  } else if (isRequestContinuous) {
    if (sensorTypeIsOneShot(sensorType)) {
      success = false;
      LOGE("Invalid continuous request for a one-shot sensor.");
    }
  } else if (isRequestOneShot) {
    if (!sensorTypeIsOneShot(sensorType)) {
      success = false;
      LOGE("Invalid one-shot request for a continuous sensor.");
    }
  }
  return success;
}

void flushTimerCallback(uint16_t /* eventType */, void * /* data */) {
  // TODO: Fatal error here since some platforms may not be able to handle
  //       timeouts gracefully. Modify this implementation to drop flush
  //       requests and handle stale responses in the future appropriately.
  FATAL_ERROR("Flush request timed out");
}

}  // namespace

SensorRequestManager::SensorRequestManager() {
  mSensorRequests.resize(mSensorRequests.capacity());

  DynamicVector<Sensor> sensors;
  sensors.reserve(8);  // Avoid some initial reallocation churn
  if (!PlatformSensor::getSensors(&sensors)) {
    LOGE("Failed to query the platform for sensors");
  } else if (sensors.empty()) {
    LOGW("Platform returned zero sensors");
  } else {
    for (size_t i = 0; i < sensors.size(); i++) {
      SensorType sensorType = sensors[i].getSensorType();
      size_t sensorIndex = getSensorTypeArrayIndex(sensorType);

      if (sensorType == SensorType::Unknown) {
        LOGE("Invalid sensor type");
      } else if (sensors[i].getMinInterval() == 0) {
        LOGE("Invalid sensor minInterval: %s", getSensorTypeName(sensorType));
      } else {
        mSensorRequests[sensorIndex].setSensor(std::move(sensors[i]));
        LOGD("Found sensor: %s", getSensorTypeName(sensorType));
      }
    }
  }
}

SensorRequestManager::~SensorRequestManager() {
  for (size_t i = 0; i < mSensorRequests.size(); i++) {
    // Disable sensors that have been enabled previously.
    if (mSensorRequests[i].isSensorSupported()) {
      mSensorRequests[i].removeAll();
    }
  }
}

bool SensorRequestManager::getSensorHandle(SensorType sensorType,
                                           uint32_t *sensorHandle) const {
  CHRE_ASSERT(sensorHandle);

  bool sensorHandleIsValid = false;
  if (sensorType == SensorType::Unknown) {
    LOGW("Querying for unknown sensor type");
  } else {
    size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
    sensorHandleIsValid = mSensorRequests[sensorIndex].isSensorSupported();
    if (sensorHandleIsValid) {
      *sensorHandle = getSensorHandleFromSensorType(sensorType);
    }
  }

  return sensorHandleIsValid;
}

bool SensorRequestManager::setSensorRequest(Nanoapp *nanoapp,
    uint32_t sensorHandle, const SensorRequest& sensorRequest) {
  CHRE_ASSERT(nanoapp);

  // Validate the input to ensure that a valid handle has been provided.
  SensorType sensorType = getSensorTypeFromSensorHandle(sensorHandle);
  if (sensorType == SensorType::Unknown) {
    LOGW("Attempting to configure an invalid sensor handle");
    return false;
  }

  // Ensure that the runtime is aware of this sensor type.
  size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
  SensorRequests& requests = mSensorRequests[sensorIndex];
  if (!requests.isSensorSupported()) {
    LOGW("Attempting to configure non-existent sensor");
    return false;
  }

  const Sensor& sensor = requests.getSensor();
  if (!isSensorRequestValid(sensor, sensorRequest)) {
    return false;
  }

  size_t requestIndex;
  uint16_t eventType = getSampleEventTypeForSensorType(sensorType);
  bool nanoappHasRequest = (requests.find(nanoapp->getInstanceId(),
                                          &requestIndex) != nullptr);

  bool success;
  bool requestChanged;
  if (sensorRequest.getMode() == SensorMode::Off) {
    if (nanoappHasRequest) {
      // The request changes the mode to off and there was an existing request.
      // The existing request is removed from the multiplexer. The nanoapp is
      // unregistered from events of this type if this request was successful.
      success = requests.remove(requestIndex, &requestChanged);
      if (success) {
        nanoapp->unregisterForBroadcastEvent(eventType);

        uint16_t biasEventType;
        if (getSensorBiasEventType(sensorType, &biasEventType)) {
          // Per API requirements, turn off bias reporting when unsubscribing
          // from the sensor.
          nanoapp->unregisterForBroadcastEvent(biasEventType);
        }
      }
    } else {
      // The sensor is being configured to Off, but is already Off (there is no
      // existing request). We assign to success to be true and no other
      // operation is required.
      requestChanged = false;
      success = true;
    }
  } else if (!nanoappHasRequest) {
    // The request changes the mode to the enabled state and there was no
    // existing request. The request is newly created and added to the
    // multiplexer. The nanoapp is registered for events if this request was
    // successful.
    success = requests.add(sensorRequest, &requestChanged);
    if (success) {
      nanoapp->registerForBroadcastEvent(eventType);

      // Per API requirements, turn on bias reporting for calibrated sensors
      // by default when subscribed.
      uint16_t biasEventType;
      if (getSensorBiasEventType(sensorType, &biasEventType)
          && sensorTypeIsCalibrated(sensorType)) {
        nanoapp->registerForBroadcastEvent(biasEventType);
      }

      // Deliver last valid event to new clients of on-change sensors
      if (sensorTypeIsOnChange(sensor.getSensorType())
          && sensor.getLastEvent() != nullptr) {
        EventLoopManagerSingleton::get()->getEventLoop()
            .postEvent(getSampleEventTypeForSensorType(sensorType),
                       sensor.getLastEvent(), nullptr, kSystemInstanceId,
                       nanoapp->getInstanceId());
      }
    }
  } else {
    // The request changes the mode to the enabled state and there was an
    // existing request. The existing request is updated.
    success = requests.update(requestIndex, sensorRequest, &requestChanged);
  }

  if (requestChanged) {
    // TODO: Send an event to nanoapps to indicate the rate change.
  }

  return success;
}

bool SensorRequestManager::getSensorInfo(uint32_t sensorHandle,
                                         const Nanoapp& nanoapp,
                                         struct chreSensorInfo *info) const {
  CHRE_ASSERT(info);

  bool success = false;

  // Validate the input to ensure that a valid handle has been provided.
  SensorType sensorType = getSensorTypeFromSensorHandle(sensorHandle);
  if (sensorType == SensorType::Unknown) {
    LOGW("Attempting to access sensor with an invalid handle %" PRIu32,
         sensorHandle);
  } else {
    size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
    if (!mSensorRequests[sensorIndex].isSensorSupported()) {
      LOGW("Attempting to get sensor info for unsupported sensor handle %"
           PRIu32, sensorHandle);
    } else {
      // Platform-independent properties.
      info->sensorType = getUnsignedIntFromSensorType(sensorType);
      info->isOnChange = sensorTypeIsOnChange(sensorType);
      info->isOneShot  = sensorTypeIsOneShot(sensorType);
      info->reportsBiasEvents = sensorTypeReportsBias(sensorType);
      info->unusedFlags = 0;

      // Platform-specific properties.
      const Sensor& sensor = mSensorRequests[sensorIndex].getSensor();
      info->sensorName = sensor.getSensorName();

      // minInterval was added in CHRE API v1.1 - do not attempt to populate for
      // nanoapps targeting v1.0 as their struct will not be large enough
      if (nanoapp.getTargetApiVersion() >= CHRE_API_VERSION_1_1) {
        info->minInterval = sensor.getMinInterval();
      }

      success = true;
    }
  }

  return success;
}

bool SensorRequestManager::removeAllRequests(SensorType sensorType) {
  bool success = false;
  if (sensorType == SensorType::Unknown) {
    LOGW("Attempting to remove all requests of an invalid sensor type");
  } else {
    size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
    SensorRequests& requests = mSensorRequests[sensorIndex];
    uint16_t eventType = getSampleEventTypeForSensorType(sensorType);

    for (const SensorRequest& request : requests.getRequests()) {
      Nanoapp *nanoapp = EventLoopManagerSingleton::get()->getEventLoop()
          .findNanoappByInstanceId(request.getInstanceId());
      if (nanoapp != nullptr) {
        nanoapp->unregisterForBroadcastEvent(eventType);
      }
    }

    success = requests.removeAll();
  }
  return success;
}

Sensor *SensorRequestManager::getSensor(SensorType sensorType) {
  Sensor *sensorPtr = nullptr;
  if (sensorType == SensorType::Unknown
      || sensorType >= SensorType::SENSOR_TYPE_COUNT) {
    LOGW("Attempting to get Sensor of an invalid SensorType %d",
         static_cast<int>(sensorType));
  } else {
    size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
    if (mSensorRequests[sensorIndex].isSensorSupported()) {
      sensorPtr = &mSensorRequests[sensorIndex].getSensor();
    }
  }
  return sensorPtr;
}

bool SensorRequestManager::getSensorSamplingStatus(
    uint32_t sensorHandle, struct chreSensorSamplingStatus *status) const {
  CHRE_ASSERT(status);

  bool success = false;
  SensorType sensorType = getSensorTypeFromSensorHandle(sensorHandle);
  if (sensorType == SensorType::Unknown) {
    LOGW("Attempting to access sensor with an invalid handle %" PRIu32,
         sensorHandle);
  } else {
    size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
    if (mSensorRequests[sensorIndex].isSensorSupported()) {
      success = mSensorRequests[sensorIndex].getSamplingStatus(status);
    }
  }
  return success;
}

const DynamicVector<SensorRequest>& SensorRequestManager::getRequests(
    SensorType sensorType) const {
  size_t sensorIndex = 0;
  if (sensorType == SensorType::Unknown
      || sensorType >= SensorType::SENSOR_TYPE_COUNT) {
    LOGW("Attempting to get requests of an invalid SensorType");
  } else {
    sensorIndex = getSensorTypeArrayIndex(sensorType);
  }
  return mSensorRequests[sensorIndex].getRequests();
}

bool SensorRequestManager::configureBiasEvents(
      Nanoapp *nanoapp, uint32_t sensorHandle, bool enable) {
  bool success = false;
  uint16_t eventType;
  SensorType sensorType = getSensorTypeFromSensorHandle(sensorHandle);
  if (getSensorBiasEventType(sensorType, &eventType)) {
    if (enable) {
      nanoapp->registerForBroadcastEvent(eventType);
    } else {
      nanoapp->unregisterForBroadcastEvent(eventType);
    }

    success = true;
  }

  return success;
}

bool SensorRequestManager::getThreeAxisBias(
    uint32_t sensorHandle, struct chreSensorThreeAxisData *bias) const {
  CHRE_ASSERT(bias != nullptr);

  bool success = false;
  if (bias != nullptr) {
    SensorType sensorType = getSensorTypeFromSensorHandle(sensorHandle);
    if (sensorType == SensorType::Unknown) {
      LOGW("Attempting to access sensor with an invalid handle %" PRIu32,
           sensorHandle);
    } else {
      size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
      if (mSensorRequests[sensorIndex].isSensorSupported()) {
        success = mSensorRequests[sensorIndex].getThreeAxisBias(bias);
      }
    }
  }

  return success;
}

bool SensorRequestManager::flushAsync(
    Nanoapp *nanoapp, uint32_t sensorHandle, const void *cookie) {
  bool success = false;

  uint32_t nanoappInstanceId = nanoapp->getInstanceId();
  SensorType sensorType = getSensorTypeFromSensorHandle(sensorHandle);
  // NOTE: One-shot sensors do not support flush per API
  if (sensorType == SensorType::Unknown || sensorTypeIsOneShot(sensorType)) {
    LOGE("Cannot flush for sensor type %" PRIu32,
         static_cast<uint32_t>(sensorType));
  } else if (mFlushRequestQueue.full()) {
    LOG_OOM();
  } else {
    mFlushRequestQueue.emplace_back(sensorType, nanoappInstanceId, cookie);
    size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
    success = (mSensorRequests[sensorIndex].makeFlushRequest(
        mFlushRequestQueue.back()) == CHRE_ERROR_NONE);
    if (!success) {
      mFlushRequestQueue.pop_back();
    }
  }

  return success;
}

void SensorRequestManager::handleFlushCompleteEvent(
    uint8_t errorCode, SensorType sensorType) {
  struct CallbackState {
    uint8_t errorCode;
    SensorType sensorType;
  };

  // Enables passing data through void pointer to avoid allocation.
  union NestedCallbackState {
    void *eventData;
    CallbackState callbackState;
  };
  static_assert(sizeof(NestedCallbackState) == sizeof(void *),
                "Size of NestedCallbackState must equal that of void *");

  NestedCallbackState state = {};
  state.callbackState.errorCode = errorCode;
  state.callbackState.sensorType = sensorType;

  auto callback = [](uint16_t /* eventType */, void *eventData) {
    NestedCallbackState nestedState;
    nestedState.eventData = eventData;
    EventLoopManagerSingleton::get()->getSensorRequestManager()
        .handleFlushCompleteEventSync(nestedState.callbackState.errorCode,
                                      nestedState.callbackState.sensorType);
  };

  EventLoopManagerSingleton::get()->deferCallback(
      SystemCallbackType::SensorFlushComplete, state.eventData, callback);
}

void SensorRequestManager::logStateToBuffer(char *buffer, size_t *bufferPos,
                                            size_t bufferSize) const {
  debugDumpPrint(buffer, bufferPos, bufferSize, "\nSensors:\n");
  for (uint8_t i = 0; i < static_cast<uint8_t>(SensorType::SENSOR_TYPE_COUNT);
       i++) {
    SensorType sensor = static_cast<SensorType>(i);
    if (sensor != SensorType::Unknown) {
      for (const auto& request : getRequests(sensor)) {
        debugDumpPrint(buffer, bufferPos, bufferSize, " %s: mode=%d"
                       " interval(ns)=%" PRIu64 " latency(ns)=%"
                       PRIu64 " nanoappId=%" PRIu32 "\n",
                       getSensorTypeName(sensor), request.getMode(),
                       request.getInterval().toRawNanoseconds(),
                       request.getLatency().toRawNanoseconds(),
                       request.getInstanceId());
      }
    }
  }
}

void SensorRequestManager::postFlushCompleteEvent(
    uint32_t sensorHandle, uint8_t errorCode, const FlushRequest& request) {
  auto *event = memoryAlloc<chreSensorFlushCompleteEvent>();
  if (event == nullptr) {
    LOG_OOM();
  } else {
    event->sensorHandle = sensorHandle;
    event->errorCode = errorCode;
    event->cookie = request.cookie;
    memset(event->reserved, 0, sizeof(event->reserved));

    EventLoopManagerSingleton::get()->getEventLoop().postEventOrFree(
        CHRE_EVENT_SENSOR_FLUSH_COMPLETE, event, freeEventDataCallback,
        kSystemInstanceId, request.nanoappInstanceId);
  }
}

void SensorRequestManager::dispatchNextFlushRequest(
    uint32_t sensorHandle, SensorType sensorType) {
  SensorRequests& requests = getSensorRequests(sensorType);

  for (size_t i = 0; i < mFlushRequestQueue.size(); i++) {
    const FlushRequest& request = mFlushRequestQueue[i];
    if (request.sensorType == sensorType) {
      uint8_t newRequestErrorCode = requests.makeFlushRequest(request);
      if (newRequestErrorCode == CHRE_ERROR_NONE) {
        break;
      } else {
        postFlushCompleteEvent(sensorHandle, newRequestErrorCode, request);
        mFlushRequestQueue.erase(i);
        i--;
      }
    }
  }
}

void SensorRequestManager::handleFlushCompleteEventSync(
    uint8_t errorCode, SensorType sensorType) {
  for (size_t i = 0; i < mFlushRequestQueue.size(); i++) {
    const FlushRequest& request = mFlushRequestQueue[i];
    if (request.sensorType == sensorType) {
      uint32_t sensorHandle;
      if (getSensorHandle(sensorType, &sensorHandle)) {
        SensorRequests& requests = getSensorRequests(sensorType);
        requests.cancelFlushTimer();

        postFlushCompleteEvent(sensorHandle, errorCode, request);
        mFlushRequestQueue.erase(i);
        dispatchNextFlushRequest(sensorHandle, sensorType);
      }
      break;
    }
  }
}

const SensorRequest *SensorRequestManager::SensorRequests::find(
    uint32_t instanceId, size_t *index) const {
  CHRE_ASSERT(index);

  const auto& requests = mMultiplexer.getRequests();
  for (size_t i = 0; i < requests.size(); i++) {
    const SensorRequest& sensorRequest = requests[i];
    if (sensorRequest.getInstanceId() == instanceId) {
      *index = i;
      return &sensorRequest;
    }
  }

  return nullptr;
}

bool SensorRequestManager::SensorRequests::add(const SensorRequest& request,
                                               bool *requestChanged) {
  CHRE_ASSERT(requestChanged != nullptr);
  CHRE_ASSERT(isSensorSupported());

  size_t addIndex;
  bool success = true;
  if (!mMultiplexer.addRequest(request, &addIndex, requestChanged)) {
    *requestChanged = false;
    success = false;
    LOG_OOM();
  } else if (*requestChanged) {
    success = mSensor->setRequest(mMultiplexer.getCurrentMaximalRequest());
    if (!success) {
      // Remove the newly added request since the platform failed to handle it.
      // The sensor is expected to maintain the existing request so there is no
      // need to reset the platform to the last maximal request.
      mMultiplexer.removeRequest(addIndex, requestChanged);

      // This is a roll-back operation so the maximal change in the multiplexer
      // must not have changed. The request changed state is forced to false.
      *requestChanged = false;
    }
  }

  return success;
}

bool SensorRequestManager::SensorRequests::remove(size_t removeIndex,
                                                  bool *requestChanged) {
  CHRE_ASSERT(requestChanged != nullptr);
  CHRE_ASSERT(isSensorSupported());

  bool success = true;
  mMultiplexer.removeRequest(removeIndex, requestChanged);
  if (*requestChanged) {
    success = mSensor->setRequest(mMultiplexer.getCurrentMaximalRequest());
    if (!success) {
      LOGE("SensorRequestManager failed to remove a request");

      // If the platform fails to handle this request in a debug build there is
      // likely an error in the platform. This is not strictly a programming
      // error but it does make sense to use assert semantics when a platform
      // fails to handle a request that it had been sent previously.
      CHRE_ASSERT(false);

      // The request to the platform to set a request when removing has failed
      // so the request has not changed.
      *requestChanged = false;
    }
  }

  return success;
}

bool SensorRequestManager::SensorRequests::update(size_t updateIndex,
                                                  const SensorRequest& request,
                                                  bool *requestChanged) {
  CHRE_ASSERT(requestChanged != nullptr);
  CHRE_ASSERT(isSensorSupported());

  bool success = true;
  SensorRequest previousRequest = mMultiplexer.getRequests()[updateIndex];
  mMultiplexer.updateRequest(updateIndex, request, requestChanged);
  if (*requestChanged) {
    success = mSensor->setRequest(mMultiplexer.getCurrentMaximalRequest());
    if (!success) {
      // Roll back the request since sending it to the sensor failed. The
      // request will roll back to the previous maximal. The sensor is
      // expected to maintain the existing request if a request fails so there
      // is no need to reset the platform to the last maximal request.
      mMultiplexer.updateRequest(updateIndex, previousRequest, requestChanged);

      // This is a roll-back operation so the maximal change in the multiplexer
      // must not have changed. The request changed state is forced to false.
      *requestChanged = false;
    }
  }

  return success;
}

bool SensorRequestManager::SensorRequests::removeAll() {
  CHRE_ASSERT(isSensorSupported());

  bool requestChanged;
  mMultiplexer.removeAllRequests(&requestChanged);

  bool success = true;
  if (requestChanged) {
    SensorRequest maximalRequest = mMultiplexer.getCurrentMaximalRequest();
    success = mSensor->setRequest(maximalRequest);
    if (!success) {
      LOGE("SensorRequestManager failed to remove all request");

      // If the platform fails to handle this request in a debug build there is
      // likely an error in the platform. This is not strictly a programming
      // error but it does make sense to use assert semantics when a platform
      // fails to handle a request that it had been sent previously.
      CHRE_ASSERT(false);
    }
  }
  return success;
}

uint8_t SensorRequestManager::SensorRequests::makeFlushRequest(
    const FlushRequest& request) {
  uint8_t errorCode = CHRE_ERROR;
  if (!isSensorSupported()) {
    LOGE("Cannot flush on unsupported sensor");
  } else if (mMultiplexer.getRequests().size() == 0) {
    LOGE("Cannot flush on disabled sensor");
  } else if (!isFlushRequestPending()) {
    Nanoseconds now = SystemTime::getMonotonicTime();
    Nanoseconds deadline = request.deadlineTimestamp;
    if (now >= deadline) {
      LOGE("Flush sensor %" PRIu32 " failed for nanoapp ID %" PRIu32
           ": deadline exceeded", static_cast<uint32_t>(request.sensorType),
           request.nanoappInstanceId);
      errorCode = CHRE_ERROR_TIMEOUT;
    } else if (mSensor->flushAsync()) {
      errorCode = CHRE_ERROR_NONE;
      Nanoseconds delay = deadline - now;
      mFlushRequestTimerHandle =
          EventLoopManagerSingleton::get()->setDelayedCallback(
              SystemCallbackType::SensorFlushTimeout, nullptr /* data */,
              flushTimerCallback, delay);
    }
  } else {
    // Flush request will be made once the pending request is completed.
    // Return true so that the nanoapp can wait for a result through the
    // CHRE_EVENT_SENSOR_FLUSH_COMPLETE event.
    errorCode = CHRE_ERROR_NONE;
  }

  return errorCode;
}

void SensorRequestManager::SensorRequests::cancelFlushTimer() {
  EventLoopManagerSingleton::get()->cancelDelayedCallback(
      mFlushRequestTimerHandle);
  mFlushRequestTimerHandle = CHRE_TIMER_INVALID;
}

}  // namespace chre
