/*
 * Copyright 2016 The Android Open Source Project
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

#define LOG_TAG "dumpstate"

#include "DumpstateDevice.h"

#include <android-base/properties.h>
#include <android-base/unique_fd.h>
#include <cutils/properties.h>
#include <hidl/HidlBinderSupport.h>

#include <log/log.h>
#include <string.h>

#define _SVID_SOURCE
#include <dirent.h>

#include "DumpstateUtil.h"

#define MODEM_LOG_PREFIX_PROPERTY "ro.radio.log_prefix"
#define MODEM_LOG_LOC_PROPERTY "ro.radio.log_loc"
#define MODEM_LOGGING_SWITCH "persist.radio.smlog_switch"

#define DIAG_MDLOG_PERSIST_PROPERTY "persist.sys.modem.diag.mdlog"
#define DIAG_MDLOG_PROPERTY "sys.modem.diag.mdlog"
#define DIAG_MDLOG_STATUS_PROPERTY "sys.modem.diag.mdlog_on"

#define DIAG_MDLOG_NUMBER_BUGREPORT "persist.sys.modem.diag.mdlog_br_num"

using android::os::dumpstate::CommandOptions;
using android::os::dumpstate::DumpFileToFd;
using android::os::dumpstate::PropertiesHelper;
using android::os::dumpstate::RunCommandToFd;

namespace android {
namespace hardware {
namespace dumpstate {
namespace V1_0 {
namespace implementation {

#define DIAG_LOG_PREFIX "diag_log_"

void DumpstateDevice::dumpDiagLogs(int fd, std::string srcDir, std::string destDir) {
    struct dirent **dirent_list = NULL;
    int num_entries = scandir(srcDir.c_str(),
                              &dirent_list,
                              0,
                              (int (*)(const struct dirent **, const struct dirent **)) alphasort);
    if (!dirent_list) {
        return;
    } else if (num_entries <= 0) {
        return;
    }

    int maxFileNum = android::base::GetIntProperty(DIAG_MDLOG_NUMBER_BUGREPORT, 100);
    int copiedFiles = 0;

    for (int i = num_entries - 1; i >= 0; i--) {
        ALOGD("Found %s\n", dirent_list[i]->d_name);

        if (0 != strncmp(dirent_list[i]->d_name, DIAG_LOG_PREFIX, strlen(DIAG_LOG_PREFIX))) {
            continue;
        }

        if ((copiedFiles >= maxFileNum) && (maxFileNum != -1)) {
            ALOGD("Skipped %s\n", dirent_list[i]->d_name);
            continue;
        }

        copiedFiles++;

        CommandOptions options = CommandOptions::WithTimeout(120).Build();
        std::string srcLogFile = srcDir + "/" + dirent_list[i]->d_name;
        std::string destLogFile = destDir + "/" + dirent_list[i]->d_name;

        std::string copyCmd= "/vendor/bin/cp " + srcLogFile + " " + destLogFile;

        ALOGD("Copying %s to %s\n", srcLogFile.c_str(), destLogFile.c_str());
        RunCommandToFd(fd, "CP DIAG LOGS", { "/vendor/bin/sh", "-c", copyCmd.c_str() }, options);
    }

    while (num_entries--) {
        free(dirent_list[num_entries]);
    }

    free(dirent_list);
}

void DumpstateDevice::dumpModem(int fd, int fdModem)
{
    std::string modemLogDir = android::base::GetProperty(MODEM_LOG_LOC_PROPERTY, "");
    if (modemLogDir.empty()) {
        ALOGD("No modem log place is set\n");
        return;
    }

    if (!PropertiesHelper::IsUserBuild()) {

        CommandOptions options = CommandOptions::WithTimeout(120).Build();

        RunCommandToFd(fd, "MODEM DIAG SYSTEM PROPERTIES",
                       { "/vendor/bin/sh", "-c", "getprop", "|", "grep", "sys.modem.diag" }, options);

        std::string modemLogAllDir = modemLogDir + "/modem_log";
        std::string diagLogDir = "/data/vendor/radio/diag_logs/logs";
        std::vector<std::string> rilAndNetmgrLogs
            {
              "/data/vendor/radio/ril_log",
              "/data/vendor/radio/ril_log_old",
              "/data/vendor/netmgr/netmgr_log",
              "/data/vendor/netmgr/netmgr_log_old",
              "/data/vendor/radio/power_anomaly_data.txt",
              "/data/vendor/radio/diag_logs/diag_trace.txt",
              "/data/vendor/radio/diag_logs/diag_trace_old.txt",
            };

        std::string modemLogMkDirCmd= "/vendor/bin/mkdir -p " + modemLogAllDir;
        RunCommandToFd(fd, "MKDIR MODEM LOG", { "/vendor/bin/sh", "-c", modemLogMkDirCmd.c_str()}, options);

        bool smlogEnabled = android::base::GetBoolProperty(MODEM_LOGGING_SWITCH, false) &&
            !access("/vendor/bin/smlog_dump", X_OK);

        bool diagLogEnabled = android::base::GetBoolProperty(DIAG_MDLOG_PERSIST_PROPERTY, false);

        if (smlogEnabled) {
            RunCommandToFd(fd, "SMLOG DUMP", { "smlog_dump", "-d", "-o", modemLogAllDir.c_str() }, options);
        } else if (diagLogEnabled) {
            bool diagLogStarted = android::base::GetBoolProperty(DIAG_MDLOG_STATUS_PROPERTY, false);

            if (diagLogStarted) {
                android::base::SetProperty(DIAG_MDLOG_PROPERTY, "false");
                ALOGD("Stopping diag_mdlog...\n");
            } else {
                ALOGD("diag_mdlog is not running\n");
            }

            for (int i = 0; i < 30; i++) {
                if (!android::base::GetBoolProperty(DIAG_MDLOG_STATUS_PROPERTY, false)) {
                    ALOGD("diag_mdlog exited\n");
                    sleep(1);
                    break;
                }

                sleep(1);
            }

            dumpDiagLogs(fd, diagLogDir, modemLogAllDir);

            if (diagLogStarted) {
                ALOGD("Restarting diag_mdlog...\n");
                android::base::SetProperty(DIAG_MDLOG_PROPERTY, "true");
            }
        }

        for (const auto& logFile : rilAndNetmgrLogs)
        {
            std::string copyCmd= "/vendor/bin/cp " + logFile + " " + modemLogAllDir;
            RunCommandToFd(fd, "CP MODEM LOG", { "/vendor/bin/sh", "-c", copyCmd.c_str() }, options);
        }

        std::string filePrefix = android::base::GetProperty(MODEM_LOG_PREFIX_PROPERTY, "");

        if (!filePrefix.empty()) {
            std::string modemLogCombined = modemLogDir + "/" + filePrefix + "all.tar";
            std::string modemLogTarCmd= "/vendor/bin/tar cvf " + modemLogCombined + " -C " + modemLogAllDir + " .";
            RunCommandToFd(fd, "TAR LOG", { "/vendor/bin/sh", "-c", modemLogTarCmd.c_str()}, options);

            std::string modemLogPermCmd= "/vendor/bin/chmod a+rw " + modemLogCombined;
            RunCommandToFd(fd, "CHG PERM", { "/vendor/bin/sh", "-c", modemLogPermCmd.c_str()}, options);

            std::vector<uint8_t> buffer(65536);
            android::base::unique_fd fdLog(TEMP_FAILURE_RETRY(open(modemLogCombined.c_str(), O_RDONLY | O_CLOEXEC | O_NONBLOCK)));

            if (fdLog >= 0) {
                while (1) {
                    ssize_t bytes_read = TEMP_FAILURE_RETRY(read(fdLog, buffer.data(), buffer.size()));

                    if (bytes_read == 0) {
                        break;
                    } else if (bytes_read < 0) {
                        ALOGD("read(%s): %s\n", modemLogCombined.c_str(), strerror(errno));
                        break;
                    }

                    ssize_t result = TEMP_FAILURE_RETRY(write(fdModem, buffer.data(), bytes_read));

                    if (result != bytes_read) {
                        ALOGD("Failed to write %ld bytes, actually written: %ld", bytes_read, result);
                        break;
                    }
                }
            }

            std::string modemLogClearCmd= "/vendor/bin/rm -r " + modemLogAllDir;
            RunCommandToFd(fd, "RM MODEM DIR", { "/vendor/bin/sh", "-c", modemLogClearCmd.c_str()}, options);
            RunCommandToFd(fd, "RM LOG", { "/vendor/bin/rm", modemLogCombined.c_str()}, options);
        }
    }
}

static void DumpTouch(int fd) {
    if (!access("/sys/android_touch", R_OK)) {
        DumpFileToFd(fd, "Synaptics touch firmware version",
                     "/sys/android_touch/vendor");
        DumpFileToFd(fd, "Synaptics touch firmware config",
                     "/sys/android_touch/config");
    }
    if (!access("/sys/class/input/ftm4_touch", R_OK)) {
        DumpFileToFd(fd, "STM touch firmware config",
                     "/sys/class/input/ftm4_touch/version");
        DumpFileToFd(fd, "STM touch VR Mode",
                     "/sys/class/input/ftm4_touch/vrmode");
    }
}

// Methods from ::android::hardware::dumpstate::V1_0::IDumpstateDevice follow.
Return<void> DumpstateDevice::dumpstateBoard(const hidl_handle& handle) {
    // Exit when dump is completed since this is a lazy HAL.
    addPostCommandTask([]() {
        exit(0);
    });

    if (handle == nullptr || handle->numFds < 1) {
        ALOGE("no FDs\n");
        return Void();
    }

    int fd = handle->data[0];
    if (fd < 0) {
        ALOGE("invalid FD: %d\n", handle->data[0]);
        return Void();
    }

    if (handle->numFds < 2) {
        ALOGE("no FD for modem\n");
    }
    else {
        int fdModem = handle->data[1];
        dumpModem(fd, fdModem);
    }
    RunCommandToFd(fd, "VENDOR PROPERTIES", {"/vendor/bin/getprop"});
    DumpFileToFd(fd, "SoC serial number", "/sys/devices/soc0/serial_number");
    DumpFileToFd(fd, "CPU present", "/sys/devices/system/cpu/present");
    DumpFileToFd(fd, "CPU online", "/sys/devices/system/cpu/online");
    DumpFileToFd(fd, "UFS model", "/sys/block/sda/device/model");
    DumpFileToFd(fd, "UFS rev", "/sys/block/sda/device/rev");
    DumpFileToFd(fd, "UFS size", "/sys/block/sda/size");
    RunCommandToFd(fd, "UFS health", {"/vendor/bin/sh", "-c", "for f in $(find /sys/kernel/debug/ufshcd0 -type f); do if [[ -r $f && -f $f ]]; then echo --- $f; cat $f; fi; done"});
    DumpFileToFd(fd, "INTERRUPTS", "/proc/interrupts");
    DumpFileToFd(fd, "RPM Stats", "/d/rpm_stats");
    DumpFileToFd(fd, "Power Management Stats", "/d/rpm_master_stats");
    DumpFileToFd(fd, "WLAN Power Stats", "/d/wlan0/power_stats");
    DumpFileToFd(fd, "LL-Stats", "/d/wlan0/ll_stats");
    DumpFileToFd(fd, "ICNSS Stats", "/d/icnss/stats");
    DumpFileToFd(fd, "SMD Log", "/d/ipc_logging/smd/log");
    RunCommandToFd(fd, "ION HEAPS", {"/vendor/bin/sh", "-c", "for d in $(ls -d /d/ion/*); do for f in $(ls $d); do echo --- $d/$f; cat $d/$f; done; done"});
    DumpFileToFd(fd, "dmabuf info", "/d/dma_buf/bufinfo");
    RunCommandToFd(fd, "Easel debug info", {"/vendor/bin/sh", "-c", "for f in `ls /sys/bus/i2c/devices/9-0008/@(*curr|temperature|vbat|total_power)`; do echo \"$f: `cat $f`\" ; done; file=/sys/devices/virtual/misc/mnh_sm/state; echo \"$file: `cat $file`\""});
    RunCommandToFd(fd, "Temperatures", {"/vendor/bin/sh", "-c", "for f in /sys/class/thermal/thermal* ; do type=`cat $f/type` ; temp=`cat $f/temp` ; echo \"$type: $temp\" ; done"});
    RunCommandToFd(fd, "Cooling Device Current State", {"/vendor/bin/sh", "-c", "for f in /sys/class/thermal/cooling* ; do type=`cat $f/type` ; temp=`cat $f/cur_state` ; echo \"$type: $temp\" ; done"});
    RunCommandToFd(fd, "CPU time-in-state", {"/vendor/bin/sh", "-c", "for cpu in /sys/devices/system/cpu/cpu*; do f=$cpu/cpufreq/stats/time_in_state; if [ ! -f $f ]; then continue; fi; echo $f:; cat $f; done"});
    RunCommandToFd(fd, "CPU cpuidle", {"/vendor/bin/sh", "-c", "for cpu in /sys/devices/system/cpu/cpu*; do for d in $cpu/cpuidle/state*; do if [ ! -d $d ]; then continue; fi; echo \"$d: `cat $d/name` `cat $d/desc` `cat $d/time` `cat $d/usage`\"; done; done"});
    DumpFileToFd(fd, "MDP xlogs", "/data/vendor/display/mdp_xlog");
    DumpFileToFd(fd, "TCPM logs", "/d/tcpm/usbpd0");
    DumpFileToFd(fd, "PD Engine", "/d/pd_engine/usbpd0");
    DumpFileToFd(fd, "smblib-usb logs", "/d/ipc_logging/smblib/log");
    DumpFileToFd(fd, "ipc-local-ports", "/d/msm_ipc_router/dump_local_ports");
    DumpFileToFd(fd, "ipc-servers", "/d/msm_ipc_router/dump_servers");
    RunCommandToFd(fd, "ipc-logs",
                   {"/vendor/bin/sh", "-c",
                    "for f in `ls /d/ipc_logging/*_IPCRTR/log` ; do echo \"------ $f\\n`cat "
                    "$f`\\n\" ; done"});
    DumpTouch(fd);
    RunCommandToFd(fd, "USB Device Descriptors", {"/vendor/bin/sh", "-c", "cd /sys/bus/usb/devices/1-1 && cat product && cat bcdDevice; cat descriptors | od -t x1 -w16 -N96"});
    DumpFileToFd(fd, "Pixel trace", "/d/tracing/instances/pixel-trace/trace");

    // Timeout after 3s
    RunCommandToFd(fd, "QSEE logs", {"/vendor/bin/sh", "-c", "/vendor/bin/timeout 3 cat /d/tzdbg/qsee_log"});
    RunCommandToFd(fd, "Power supply properties", {"/vendor/bin/sh", "-c", "for f in /sys/class/power_supply/*/uevent ; do echo \"\n------ $f\" ; cat $f ; done"});
    DumpFileToFd(fd, "Battery cycle count", "/sys/class/power_supply/bms/device/cycle_counts_bins");
    RunCommandToFd(fd, "QCOM FG SRAM", {"/vendor/bin/sh", "-c", "echo 0 > /d/fg/sram/address ; echo 500 > /d/fg/sram/count ; cat /d/fg/sram/data"});

    DumpFileToFd(fd, "WLAN FW Log Symbol Table", "/vendor/firmware/Data.msc");

    return Void();
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace dumpstate
}  // namespace hardware
}  // namespace android
