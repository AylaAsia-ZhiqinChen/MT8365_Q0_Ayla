/*
 * Copyright (C) 2014 MediaTek Inc.
 * Modification based on code covered by the mentioned copyright
 * and/or permission notice(s).
*/

package com.mediatek.server.wifi;

public class MtkScanModeNotifier {
    private static final String TAG = "MtkScanModeNotifier";

    /********************************************************
     * JNI operations
     ********************************************************/
    /* Register native functions */
    static {
        /* Native functions are defined in libwifi-service.so */
        System.loadLibrary("wifi-service");
        registerNatives();
    }

    public static void setScanMode(boolean enable) {
        setScanOnlyModeNative(enable);
    }

    private static native int registerNatives();
    /* To stop handshake with LTE when Wi-Fi UI off and scan always available enabled,
       update scan mode from framework to FW through JNI */
    private static native boolean setScanOnlyModeNative(boolean enable);
}
