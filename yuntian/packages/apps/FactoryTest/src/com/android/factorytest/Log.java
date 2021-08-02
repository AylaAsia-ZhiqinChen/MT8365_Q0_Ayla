package com.android.factorytest;

import android.text.TextUtils;

import android.os.Build;
import android.os.SystemProperties;

/**
 * 工厂测试日志信息打印类
 */
public class Log {

    /**
     * Log的TAG
     */
    private static final String TAG = "FactoryTest";

    /**
     * DEBUG Log信息打印开关
     */
    private static final boolean DEBUG = (!Build.TYPE.equals("user") || SystemProperties.getBoolean("debug.yt.factorytest", false));
    /**
     * WARN Log信息打印开关
     */
    private static final boolean WARN = (!Build.TYPE.equals("user") || SystemProperties.getBoolean("debug.yt.factorytest", false));
    /**
     * VERBOSE Log信息打印开关
     */
    private static final boolean VERBOSE = (!Build.TYPE.equals("user") || SystemProperties.getBoolean("debug.yt.factorytest", false));
    /**
     * INFO Log信息打印开关
     */
    private static final boolean INFO = true;

    public static void d(String msg) {
        if (DEBUG) {
            android.util.Log.d(TAG, msg);
        }
    }

    public static void d(String msg, Throwable tr) {
        if (DEBUG) {
            android.util.Log.d(TAG, msg, tr);
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

    public static void e(String msg) {
        android.util.Log.e(TAG, msg);
    }

    public static void e(String msg, Throwable tr) {
        android.util.Log.e(TAG, msg, tr);
    }

    public static void e(Object obj, String msg) {
        android.util.Log.e(TAG, getPrefix(obj) + msg);
    }

    public static void e(Object obj, String msg, Throwable tr) {
        android.util.Log.e(TAG, getPrefix(obj) + msg, tr);
    }

    public static void i(String msg) {
        if (INFO) {
            android.util.Log.i(TAG, msg);
        }
    }

    public static void i(String msg, Throwable tr) {
        if (INFO) {
            android.util.Log.i(TAG, msg, tr);
        }
    }

    public static void i(Object obj, String msg) {
        if (INFO) {
            android.util.Log.i(TAG, getPrefix(obj) + msg);
        }
    }

    public static void i(Object obj, String msg, Throwable tr) {
        if (INFO) {
            android.util.Log.i(TAG, getPrefix(obj) + msg, tr);
        }
    }

    public static void v(String msg) {
        if (VERBOSE) {
            android.util.Log.v(TAG, msg);
        }
    }

    public static void v(String msg, Throwable tr) {
        if (VERBOSE) {
            android.util.Log.v(TAG, msg, tr);
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

    public static void w(String msg) {
        if (WARN) {
            android.util.Log.w(TAG, msg);
        }
    }

    public static void w(String msg, Throwable tr) {
        if (WARN) {
            android.util.Log.w(TAG, msg, tr);
        }
    }

    public static void w(Object obj, String msg) {
        if (WARN) {
            android.util.Log.w(TAG, getPrefix(obj) + msg);
        }
    }

    public static void w(Object obj, String msg, Throwable tr) {
        if (WARN) {
            android.util.Log.w(TAG, getPrefix(obj) + msg, tr);
        }
    }

    /**
     * Log信息的前缀
     *
     * @param obj 输出Log的类名或字符串
     * @return 如果obj不为null且obj为字符串，则返回[字符串]；如果obj不为null且不是字符串，
     * 则返回[obj简写类名]; 如果obj为null， 则返回空字符串。
     */
    private static String getPrefix(Object obj) {
        if (obj != null && !TextUtils.isEmpty(obj.getClass().getSimpleName())) {
            if (obj instanceof String) {
                return "[" + (String) obj + "]";
            } else {
                return "[" + obj.getClass().getSimpleName() + "]";
            }
        }
        return "";
    }
}
