package com.android.agingtest;

import android.text.TextUtils;

public class Log {

    private static final String TAG = "AgingTest";
    private static final boolean FOCRE_DEBUG = true;
    private static final boolean DEBUG = android.util.Log.isLoggable(TAG, android.util.Log.DEBUG) || FOCRE_DEBUG;
    private static final boolean VERBOSE = android.util.Log.isLoggable(TAG, android.util.Log.VERBOSE) || FOCRE_DEBUG;

    public static void d(String tag, String msg) {
        if (DEBUG) {
            android.util.Log.d(TAG, delimit(tag) + msg);
        }
    }

    public static void d(String tag, String msg, Throwable tr) {
        if (DEBUG) {
            android.util.Log.d(TAG, delimit(tag) + msg, tr);
        }
    }

    public static void d(Object obj, String msg) {
        if (DEBUG) {
            android.util.Log.d(TAG, getPrefix(obj) + msg);
        }
    }

    public static void d(Object obj, String msg, Throwable tr) {
        if (DEBUG) {
            android.util.Log.d(TAG, getPrefix(obj) + msg, tr);
        }
    }

    public static void v(String tag, String msg) {
        if (VERBOSE) {
            android.util.Log.v(TAG, delimit(tag) + msg);
        }
    }

    public static void v(String tag, String msg, Throwable tr) {
        if (VERBOSE) {
            android.util.Log.v(TAG, delimit(tag) + msg, tr);
        }
    }

    public static void v(Object obj, String msg) {
        if (VERBOSE) {
            android.util.Log.v(TAG, getPrefix(obj) + msg);
        }
    }

    public static void v(Object obj, String msg, Throwable tr) {
        if (VERBOSE) {
            android.util.Log.v(TAG, getPrefix(obj) + msg, tr);
        }
    }

    public static void i(String tag, String msg) {
        android.util.Log.i(TAG, delimit(tag) + msg);
    }

    public static void i(String tag, String msg, Throwable tr) {
        android.util.Log.i(TAG, delimit(tag) + msg, tr);
    }

    public static void i(Object obj, String msg) {
        android.util.Log.i(TAG, getPrefix(obj) + msg);
    }

    public static void i(Object obj, String msg, Throwable tr) {
        android.util.Log.i(TAG, getPrefix(obj) + msg, tr);
    }

    public static void e(String tag, String msg) {
        android.util.Log.e(TAG, delimit(tag) + msg);
    }

    public static void e(String tag, String msg, Throwable tr) {
        android.util.Log.e(TAG, delimit(tag) + msg, tr);
    }

    public static void e(Object obj, String msg) {
        android.util.Log.e(TAG, getPrefix(obj) + msg);
    }

    public static void e(Object obj, String msg, Throwable tr) {
        android.util.Log.e(TAG, getPrefix(obj) + msg, tr);
    }

    public static void w(String tag, String msg) {
        android.util.Log.w(TAG, delimit(tag) + msg);
    }

    public static void w(String tag, String msg, Throwable tr) {
        android.util.Log.w(TAG, delimit(tag) + msg, tr);
    }

    public static void w(Object obj, String msg) {
        android.util.Log.w(TAG, getPrefix(obj) + msg);
    }

    public static void w(Object obj, String msg, Throwable tr) {
        android.util.Log.w(TAG, getPrefix(obj) + msg, tr);
    }

    private static String delimit(String tag) {
        String result = "";
        if (!TextUtils.isEmpty(tag)) {
            result = "[" + tag + "]";
        }
        return result;
    }

    private static String getPrefix(Object obj) {
        String result = "";
        if (obj != null) {
            String simpleName = obj.getClass().getSimpleName();
            if (!TextUtils.isEmpty(simpleName)) {
                result = "[" + simpleName + "]";
            }
        }
        return result;
    }
}
