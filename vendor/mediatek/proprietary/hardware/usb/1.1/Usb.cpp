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

#define LOG_TAG "android.hardware.usb@1.1-service-mediatek"

#include <android-base/logging.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <chrono>
#include <dirent.h>
#include <pthread.h>
#include <regex>
#include <stdio.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

#include <cutils/uevent.h>
#include <sys/epoll.h>
#include <utils/Errors.h>
#include <utils/StrongPointer.h>

#include "Usb.h"

namespace android {
namespace hardware {
namespace usb {
namespace V1_1 {
namespace implementation {

// Set by the signal handler to destroy the thread
volatile bool destroyThread;

int32_t readFile(const std::string &filename, std::string *contents) {
  FILE *fp;
  ssize_t read = 0;
  char *line = NULL;
  size_t len = 0;

  fp = fopen(filename.c_str(), "r");
  if (fp != NULL) {
    if ((read = getline(&line, &len, fp)) != -1) {
      char *pos;
      if ((pos = strchr(line, '\n')) != NULL) *pos = '\0';
      *contents = line;
    }
    free(line);
    fclose(fp);
    return 0;
  } else {
    ALOGE("fopen failed");
  }

  return -1;
}

std::string appendRoleNodeHelper(const std::string &portName,
                                 PortRoleType type) {
  std::string node("/sys/class/dual_role_usb/" + portName);

  switch (type) {
    case PortRoleType::DATA_ROLE:
      return node + "/data_role";
    case PortRoleType::POWER_ROLE:
      return node + "/power_role";
    default:
      return node + "/mode";
  }
}

bool canSwitchRoleHelper(const std::string /*&portName*/, PortRoleType /*type*/) {
    std::string filename;
    std::string supportsPD;

    filename = "/sys/class/tcpc/type_c_port0/pe_ready";

    if (!readFile(filename, &supportsPD)) {
        if (supportsPD == "yes") {
            return true;
        }
    }
    return false;
}

std::string convertRoletoString(PortRole role) {
  if (role.type == PortRoleType::POWER_ROLE) {
    if (role.role == static_cast<uint32_t>(PortPowerRole::SOURCE))
      return "source";
    else if (role.role == static_cast<uint32_t>(PortPowerRole::SINK))
      return "sink";
  } else if (role.type == PortRoleType::DATA_ROLE) {
    if (role.role == static_cast<uint32_t>(PortDataRole::HOST)) return "host";
    if (role.role == static_cast<uint32_t>(PortDataRole::DEVICE))
      return "device";
  } else if (role.type == PortRoleType::MODE) {
    if (role.role == static_cast<uint32_t>(PortMode_1_1::UFP)) return "ufp";
    if (role.role == static_cast<uint32_t>(PortMode_1_1::DFP)) return "dfp";
  }
  return "none";
}

void switchToDrp(const std::string &portName) {
  std::string filename =
      appendRoleNodeHelper(std::string(portName.c_str()), PortRoleType::MODE);
  FILE *fp;

  if (filename != "") {
    fp = fopen(filename.c_str(), "w");
    if (fp != NULL) {
      int ret = fputs("dfp", fp);
      fclose(fp);
      if (ret == EOF)
        ALOGE("Fatal: Error while switching back to drp");
    } else {
      ALOGE("Fatal: Cannot open file to switch back to drp");
    }
  } else {
    ALOGE("Fatal: invalid node type");
  }
}

Return<void> Usb::switchRole(const hidl_string &portName,
                             const PortRole &newRole) {
  std::string filename =
      appendRoleNodeHelper(std::string(portName.c_str()), newRole.type);
  bool canSwitch =
      canSwitchRoleHelper(std::string(portName.c_str()), newRole.type);
  std::string written;
  FILE *fp;
  bool roleSwitch = false;
  int retry_times = 20;

  if (filename == "") {
    ALOGE("Fatal: invalid node type");
    return Void();
  }

  pthread_mutex_lock(&mRoleSwitchLock);

  while (retry_times > 0 && canSwitch) {
    ALOGI("filename write: %s role:%s", filename.c_str(),
          convertRoletoString(newRole).c_str());

    fp = fopen(filename.c_str(), "w");
    if (fp != NULL) {
      int ret = fputs(convertRoletoString(newRole).c_str(), fp);
      fclose(fp);
      if ((ret != EOF) && !readFile(filename, &written)) {
        ALOGI("written: %s", written.c_str());
        if (written == convertRoletoString(newRole)) {
          roleSwitch = true;
          break;
        } else {
          ALOGE("Role switch failed");
        }
      } else {
        ALOGE("failed to update the new role");
      }
    } else {
      ALOGE("fopen failed");
    }

    ALOGI("retry_times: %d", retry_times);
    retry_times--;
    usleep(50*1000); // 50 ms
  }

  pthread_mutex_lock(&mLock);
  if (mCallback_1_0 != NULL) {
    Return<void> ret =
        mCallback_1_0->notifyRoleSwitchStatus(portName, newRole,
        roleSwitch ? Status::SUCCESS : Status::ERROR);
    if (!ret.isOk())
      ALOGE("RoleSwitchStatus error %s", ret.description().c_str());
  } else {
    ALOGE("Not notifying the userspace. Callback is not set");
  }
  pthread_mutex_unlock(&mLock);
  pthread_mutex_unlock(&mRoleSwitchLock);

  return Void();
}

Status getCurrentRoleHelper(const std::string &portName, bool connected,
                            PortRoleType type, uint32_t *currentRole) {
  std::string filename;
  std::string roleName;

  // Mode

  if (type == PortRoleType::POWER_ROLE) {
    filename = "/sys/class/dual_role_usb/" + portName + "/power_role";
    *currentRole = static_cast<uint32_t>(PortPowerRole::NONE);
  } else if (type == PortRoleType::DATA_ROLE) {
    filename = "/sys/class/dual_role_usb/" + portName + "/data_role";
    *currentRole = static_cast<uint32_t>(PortDataRole::NONE);
  } else if (type == PortRoleType::MODE) {
    filename = "/sys/class/dual_role_usb/" + portName + "/mode";
    *currentRole = static_cast<uint32_t>(PortMode_1_1::NONE);
  } else {
    return Status::ERROR;
  }

  if (!connected) return Status::SUCCESS;


  if (readFile(filename, &roleName)) {
    ALOGE("getCurrentRole: Failed to open filesystem node: %s",
          filename.c_str());
    return Status::ERROR;
  }

  if (roleName == "dfp")
      *currentRole = static_cast<uint32_t>(PortMode_1_1::DFP);
  else if (roleName == "ufp")
      *currentRole = static_cast<uint32_t>(PortMode_1_1::UFP);
  else if (roleName == "source")
      *currentRole = static_cast<uint32_t>(PortPowerRole::SOURCE);
  else if (roleName == "sink")
      *currentRole = static_cast<uint32_t>(PortPowerRole::SINK);
  else if (roleName == "host")
      *currentRole = static_cast<uint32_t>(PortDataRole::HOST);
  else if (roleName == "device")
      *currentRole = static_cast<uint32_t>(PortDataRole::DEVICE);
  else if (roleName != "none") {
    /* case for none has already been addressed.
     * so we check if the role isnt none.
     */
    return Status::UNRECOGNIZED_ROLE;
  }
  return Status::SUCCESS;
}

Status getTypeCPortNamesHelper(std::unordered_map<std::string, bool> *names) {
  DIR *dp;

  dp = opendir("/sys/class/dual_role_usb");
  if (dp != NULL) {
    struct dirent *ep;

    while ((ep = readdir(dp))) {
      if (ep->d_type == DT_LNK) {
          std::unordered_map<std::string, bool>::const_iterator portName =
              names->find(ep->d_name);
          if (portName == names->end()) {
            names->insert({ep->d_name, true});
          }
      }
    }
    closedir(dp);
    return Status::SUCCESS;
  }

  ALOGE("Failed to open /sys/class/dual_role_usb");
  return Status::ERROR;
}

Status getPortModeHelper(const std::string &portName, uint32_t *portMode) {
    std::string filename;
    std::string modes;

    filename = "/sys/class/dual_role_usb/" + portName + "/supported_modes";

    if (readFile(filename, &modes)) {
        ALOGE("getSupportedRoles: Failed to open filesystem node");
        return Status::ERROR;
    }

    if (modes == "ufp dfp")
        *portMode = static_cast<uint32_t>(PortMode_1_1::DRP);
    else  if (modes == "ufp")
        *portMode = static_cast<uint32_t>(PortMode_1_1::UFP);
    else if  (modes == "dfp")
        *portMode = static_cast<uint32_t>(PortMode_1_1::DFP);
    else
        return Status::UNRECOGNIZED_ROLE;

    return Status::SUCCESS;
}

/*
 * Reuse the same method for both V1_0 and V1_1 callback objects.
 * The caller of this method would reconstruct the V1_0::PortStatus
 * object if required.
 */
Status getPortStatusHelper(hidl_vec<PortStatus_1_1> *currentPortStatus_1_1,
    bool V1_0) {
  std::unordered_map<std::string, bool> names;
  Status result = getTypeCPortNamesHelper(&names);
  int i = -1;

  if (result == Status::SUCCESS) {
    currentPortStatus_1_1->resize(names.size());
    for (std::pair<std::string, bool> port : names) {
      i++;
      ALOGI("%s", port.first.c_str());
      (*currentPortStatus_1_1)[i].status.portName = port.first;

      uint32_t currentRole;
      if (getCurrentRoleHelper(port.first, port.second,
                               PortRoleType::POWER_ROLE,
                               &currentRole) == Status::SUCCESS) {
        (*currentPortStatus_1_1)[i].status.currentPowerRole =
            static_cast<PortPowerRole>(currentRole);
      } else {
        ALOGE("Error while retreiving portNames");
        goto done;
      }

      if (getCurrentRoleHelper(port.first, port.second, PortRoleType::DATA_ROLE,
                               &currentRole) == Status::SUCCESS) {
        (*currentPortStatus_1_1)[i].status.currentDataRole =
            static_cast<PortDataRole>(currentRole);
      } else {
        ALOGE("Error while retreiving current port role");
        goto done;
      }

      if (getCurrentRoleHelper(port.first, port.second, PortRoleType::MODE,
                               &currentRole) == Status::SUCCESS) {
        (*currentPortStatus_1_1)[i].currentMode =
            static_cast<PortMode_1_1>(currentRole);
        (*currentPortStatus_1_1)[i].status.currentMode =
            static_cast<V1_0::PortMode>(currentRole);
      } else {
        ALOGE("Error while retreiving current data role");
        goto done;
      }

      (*currentPortStatus_1_1)[i].status.canChangeMode =
          port.second ? canSwitchRoleHelper(port.first, PortRoleType::MODE)
                      : false;
      (*currentPortStatus_1_1)[i].status.canChangeDataRole =
          port.second ? canSwitchRoleHelper(port.first, PortRoleType::DATA_ROLE)
                      : false;
      (*currentPortStatus_1_1)[i].status.canChangePowerRole =
          port.second
              ? canSwitchRoleHelper(port.first, PortRoleType::POWER_ROLE)
              : false;

      ALOGI("connected:%d canChangeMode:%d canChagedata:%d canChangePower:%d",
            port.second, (*currentPortStatus_1_1)[i].status.canChangeMode,
            (*currentPortStatus_1_1)[i].status.canChangeDataRole,
            (*currentPortStatus_1_1)[i].status.canChangePowerRole);

      uint32_t portMode;
      if (getPortModeHelper(port.first, &portMode)
                  != Status::SUCCESS) {
          ALOGE("Error while retrieving port modes");
          goto done;
      }

      if (V1_0) {
        (*currentPortStatus_1_1)[i].status.supportedModes =
            static_cast<V1_0::PortMode>(portMode);
      } else {
        (*currentPortStatus_1_1)[i].supportedModes = portMode;
        (*currentPortStatus_1_1)[i].status.supportedModes = V1_0::PortMode::NONE;
        (*currentPortStatus_1_1)[i].status.currentMode = V1_0::PortMode::NONE;
      }
    }
    return Status::SUCCESS;
  }
done:
  return Status::ERROR;
}

Return<void> Usb::queryPortStatus() {
  hidl_vec<PortStatus_1_1> currentPortStatus_1_1;
  hidl_vec<V1_0::PortStatus> currentPortStatus;
  Status status;
  sp<IUsbCallback> callback_V1_1 = IUsbCallback::castFrom(mCallback_1_0);

  pthread_mutex_lock(&mLock);
  if (mCallback_1_0 != NULL) {
    if (callback_V1_1 != NULL) {
      status = getPortStatusHelper(&currentPortStatus_1_1, false);
    } else {
      status = getPortStatusHelper(&currentPortStatus_1_1, true);
      currentPortStatus.resize(currentPortStatus_1_1.size());
      for (unsigned long i = 0; i < currentPortStatus_1_1.size(); i++)
        currentPortStatus[i] = currentPortStatus_1_1[i].status;
    }

    Return<void> ret;

    if (callback_V1_1 != NULL)
      ret = callback_V1_1->notifyPortStatusChange_1_1(currentPortStatus_1_1, status);
    else
      ret = mCallback_1_0->notifyPortStatusChange(currentPortStatus, status);

    if (!ret.isOk())
      ALOGE("queryPortStatus_1_1 error %s", ret.description().c_str());
  } else {
    ALOGI("Notifying userspace skipped. Callback is NULL");
  }
  pthread_mutex_unlock(&mLock);

  return Void();
}

struct data {
  int uevent_fd;
  android::hardware::usb::V1_1::implementation::Usb *usb;
};

static void uevent_event(uint32_t /*epevents*/, struct data *payload) {
  char msg[UEVENT_MSG_LEN + 2];
  char *cp;
  int n;

  n = uevent_kernel_multicast_recv(payload->uevent_fd, msg, UEVENT_MSG_LEN);
  if (n <= 0) return;
  if (n >= UEVENT_MSG_LEN) /* overflow -- discard */
    return;

  msg[n] = '\0';
  msg[n + 1] = '\0';
  cp = msg;

  while (*cp) {
    if (!strcmp(cp, "SUBSYSTEM=dual_role_usb")) {
      hidl_vec<PortStatus_1_1> currentPortStatus_1_1;
      ALOGI("uevent received %s", cp);
      pthread_mutex_lock(&payload->usb->mLock);
      if (payload->usb->mCallback_1_0 != NULL) {
        sp<IUsbCallback> callback_V1_1 = IUsbCallback::castFrom(payload->usb->mCallback_1_0);
        Return<void> ret;

        // V1_1 callback
        if (callback_V1_1 != NULL) {
          Status status = getPortStatusHelper(&currentPortStatus_1_1, false);
          ret = callback_V1_1->notifyPortStatusChange_1_1(
              currentPortStatus_1_1, status);
        } else { // V1_0 callback
          Status status = getPortStatusHelper(&currentPortStatus_1_1, true);

          /*
           * Copying the result from getPortStatusHelper
           * into V1_0::PortStatus to pass back through
           * the V1_0 callback object.
           */
          hidl_vec<V1_0::PortStatus> currentPortStatus;
          currentPortStatus.resize(currentPortStatus_1_1.size());
          for (unsigned long i = 0; i < currentPortStatus_1_1.size(); i++)
            currentPortStatus[i] = currentPortStatus_1_1[i].status;

          ret = payload->usb->mCallback_1_0->notifyPortStatusChange(
              currentPortStatus, status);
        }
        if (!ret.isOk()) ALOGE("error %s", ret.description().c_str());
      } else {
        ALOGI("Notifying userspace skipped. Callback is NULL");
      }
      pthread_mutex_unlock(&payload->usb->mLock);

      //Role switch is not in progress and port is in disconnected state
      if (!pthread_mutex_trylock(&payload->usb->mRoleSwitchLock)) {
        for (unsigned long i = 0; i < currentPortStatus_1_1.size(); i++) {
          DIR *dp = opendir(std::string("/sys/class/dual_role_usb/"
              + std::string(currentPortStatus_1_1[i].status.portName.c_str())).c_str());
          if (dp == NULL) {
              //PortRole role = {.role = static_cast<uint32_t>(PortMode::UFP)};
              switchToDrp(currentPortStatus_1_1[i].status.portName);
          } else {
              closedir(dp);
          }
        }
        pthread_mutex_unlock(&payload->usb->mRoleSwitchLock);
      }
      break;
    }
    /* advance to after the next \0 */
    while (*cp++) {}
  }
}

void *work(void *param) {
  int epoll_fd, uevent_fd;
  struct epoll_event ev;
  int nevents = 0;
  struct data payload;

  ALOGE("creating thread");

  uevent_fd = uevent_open_socket(64 * 1024, true);

  if (uevent_fd < 0) {
    ALOGE("uevent_init: uevent_open_socket failed\n");
    return NULL;
  }

  payload.uevent_fd = uevent_fd;
  payload.usb = (android::hardware::usb::V1_1::implementation::Usb *)param;

  fcntl(uevent_fd, F_SETFL, O_NONBLOCK);

  ev.events = EPOLLIN;
  ev.data.ptr = (void *)uevent_event;

  epoll_fd = epoll_create(64);
  if (epoll_fd == -1) {
    ALOGE("epoll_create failed; errno=%d", errno);
    goto error;
  }

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, uevent_fd, &ev) == -1) {
    ALOGE("epoll_ctl failed; errno=%d", errno);
    goto error;
  }

  while (!destroyThread) {
    struct epoll_event events[64];

    nevents = epoll_wait(epoll_fd, events, 64, -1);
    if (nevents == -1) {
      if (errno == EINTR) continue;
      ALOGE("usb epoll_wait failed; errno=%d", errno);
      break;
    }

    for (int n = 0; n < nevents; ++n) {
      if (events[n].data.ptr)
        (*(void (*)(int, struct data *payload))events[n].data.ptr)(
            events[n].events, &payload);
    }
  }

  ALOGI("exiting worker thread");
error:
  close(uevent_fd);

  if (epoll_fd >= 0) close(epoll_fd);

  return NULL;
}

void sighandler(int sig) {
  if (sig == SIGUSR1) {
    destroyThread = true;
    ALOGI("destroy set");
    return;
  }
  signal(SIGUSR1, sighandler);
}

Return<void> Usb::setCallback(const sp<V1_0::IUsbCallback> &callback) {

  sp<IUsbCallback> callback_V1_1 = IUsbCallback::castFrom(callback);

  if (callback != NULL)
      if (callback_V1_1 == NULL)
          ALOGI("Registering 1.0 callback");

  pthread_mutex_lock(&mLock);
  /*
   * When both the old callback and new callback values are NULL,
   * there is no need to spin off the worker thread.
   * When both the values are not NULL, we would already have a
   * worker thread running, so updating the callback object would
   * be suffice.
   */
  if ((mCallback_1_0 == NULL && callback == NULL) ||
      (mCallback_1_0 != NULL && callback != NULL)) {
    /*
     * Always store as V1_0 callback object. Type cast to V1_1
     * when the callback is actually invoked.
     */
    mCallback_1_0 = callback;
    pthread_mutex_unlock(&mLock);
    return Void();
  }

  mCallback_1_0 = callback;
  ALOGI("registering callback");

  // Kill the worker thread if the new callback is NULL.
  if (mCallback_1_0 == NULL) {
    pthread_mutex_unlock(&mLock);
    if (!pthread_kill(mPoll, SIGUSR1)) {
      pthread_join(mPoll, NULL);
      ALOGI("pthread destroyed");
    }
    return Void();
  }

  destroyThread = false;
  signal(SIGUSR1, sighandler);

  /*
   * Create a background thread if the old callback value is NULL
   * and being updated with a new value.
   */
  if (pthread_create(&mPoll, NULL, work, this)) {
    ALOGE("pthread creation failed %d", errno);
    mCallback_1_0 = NULL;
  }

  pthread_mutex_unlock(&mLock);
  return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace usb
}  // namespace hardware
}  // namespace android
