package com.mediatek.bluetooth.pbapclient;


import android.bluetooth.BluetoothPbapClient;

public class BluetoothPbapClientConstants {

    public static final int CONNECTION_STATE_DISCONNECTED = 0;
    public static final int CONNECTION_STATE_CONNECTING = 1;
    public static final int CONNECTION_STATE_CONNECTED = 2;
    public static final int CONNECTION_STATE_DISCONNECTING = 3;

    /**
     * Path to local incoming calls history object
     */
    public static final String ICH_PATH = "telecom/ich";

    /**
     * Path to local outgoing calls history object
     */
    public static final String OCH_PATH = "telecom/och";

    /**
     * Path to local missed calls history object
     */
    public static final String MCH_PATH = "telecom/mch";

    /**
     * Path to local combined calls history object
     */
    public static final String CCH_PATH = "telecom/cch";

    /**
     * Path to local main phone book object
     */
    public static final String PB_PATH = "telecom/pb";

    /**
     * Path to main phone book object stored on the phone's SIM card
     */
    public static final String SIM_PB_PATH = "SIM1/telecom/pb";

    public static final String ROOT_PATH = "telecom/";
}
