/*
 * Copyright (C) 2014 MediaTek Inc.
 * Modification based on code covered by the mentioned copyright
 * and/or permission notice(s).
*/

#define LOG_TAG "wifi-jni"

#include <ctype.h>
#include <stdlib.h>
#include <sys/klog.h>

#include <log/log.h>
#include <jni.h>
#include <nativehelper/jni_macros.h>
#include <nativehelper/JNIHelp.h>
#include <sys/socket.h>
#include <errno.h>
#include <linux/wireless.h>

#include "jni_helper.h"

#define PRIV_CMD_SIZE 512
struct ANDROID_WIFI_PRIV_CMD {
    char buf[PRIV_CMD_SIZE];
    int used_len;
    int total_len;
};

namespace android {

static jboolean android_net_wifi_setScanOnlyMode(JNIEnv* env, jclass cls, jboolean enable) {
    int ioctl_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (ioctl_sock < 0) {
        ALOGE("socket(PF_INET,SOCK_DGRAM) %d %s", ioctl_sock, strerror(errno));
        return JNI_FALSE;
    }

    struct ifreq ifr;
    ANDROID_WIFI_PRIV_CMD priv_cmd;
    char cmd[PRIV_CMD_SIZE];
    snprintf(cmd, sizeof(cmd), "set_fw_param alwaysscanen %d", enable ? 1 : 0);
    int cmd_len = strlen(cmd);

    memset(&ifr, 0, sizeof(struct ifreq));
    memset(&priv_cmd, 0, sizeof(ANDROID_WIFI_PRIV_CMD));
    strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ - 1);

    memcpy(priv_cmd.buf, cmd, cmd_len + 1);
    priv_cmd.used_len = cmd_len + 1;
    priv_cmd.total_len = PRIV_CMD_SIZE;
    ifr.ifr_data = &priv_cmd;

    int ret = ioctl(ioctl_sock, SIOCDEVPRIVATE + 1, &ifr);
    close(ioctl_sock);
    if (ret < 0) {
        ALOGI("ioctl: cmd:%s, ret:%d, error:%s", cmd, ret, strerror(errno));
        return JNI_FALSE;
    }
    ALOGI("setScanOnlyMode -> %d successfully", enable);

    return JNI_TRUE;
}

// ----------------------------------------------------------------------------

/*
 * JNI registration.
 */
static JNINativeMethod gWifiMethods[] = {
    /* name, signature, funcPtr */
    {"setScanOnlyModeNative", "(Z)Z", (void*)android_net_wifi_setScanOnlyMode},
};

/* User to register native functions */
extern "C"
jint Java_com_mediatek_server_wifi_MtkScanModeNotifier_registerNatives(JNIEnv* env, jclass clazz) {
    return jniRegisterNativeMethods(env,
            "com/mediatek/server/wifi/MtkScanModeNotifier", gWifiMethods, NELEM(gWifiMethods));
}

};  // namespace android
