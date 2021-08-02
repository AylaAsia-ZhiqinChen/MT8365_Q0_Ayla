package com.mesh.test.provisioner.util;


import android.util.Log;

public class LogUtils {

    private static final boolean DEBUG = false;

    private static final String TAG = "MeshTestProvisioner";

    public static void log(String string) {
        if(DEBUG) {
            Log.i(TAG,string);
        }
    }
}
