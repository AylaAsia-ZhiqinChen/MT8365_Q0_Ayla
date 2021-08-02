package com.verizon.remoteSimlock;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import com.android.internal.telephony.uicc.IccUtils;
import com.mediatek.internal.telephony.uicc.MtkUiccController;

public class VZWRemoteSimlockService extends Service {
    private static final String TAG = "VZWRemoteSimlockService:";

    private static final int REMOTE_SIM_UNLOCK_ERROR = 1;

    private int mRsuResult = 1;

    public void onCreate() {
        logi("onCreate, thread name = " + Thread.currentThread().getName());
        super.onCreate();
        logi("On create service done");
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        logi("onStartCommand");
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        logi("onBind");
        return mBinder;
    }

    public void onRebind(Intent intent) {
        logi("onRebind");
        super.onRebind(intent);
    }

    @Override
    public boolean onUnbind(Intent intent) {
        logi("onUnbind");
        return super.onUnbind(intent);
    }

    public void onDestroy() {
        logi("onDestroy");
        super.onDestroy();
    }

    public void onStop() {
        logi("onStop");
        stopSelf();
    }

    private IVZWRemoteSimlockService.Stub mBinder = new IVZWRemoteSimlockService.Stub() {
        @Override
        public synchronized int registerCallback(IVZWRemoteSimlockServiceCallback cb) {
            logi("registerCallback IVZWRemoteSimlockServiceCallback = " + cb);
            try {
                mRsuResult = ((MtkUiccController) MtkUiccController.getInstance()).getRsuSml()
                        .registerCallback(cb);
            } catch (NullPointerException e) {
                mRsuResult = REMOTE_SIM_UNLOCK_ERROR;
                loge("registerCallback NullPointerException");
            }
            return mRsuResult;
        }

        @Override
        public synchronized int deregisterCallback(IVZWRemoteSimlockServiceCallback cb) {
            logi("deregisterCallback IVZWRemoteSimlockServiceCallback = " + cb);
            try {
                mRsuResult = ((MtkUiccController) MtkUiccController.getInstance()).getRsuSml()
                        .deregisterCallback(cb);
            } catch (NullPointerException e) {
                mRsuResult = REMOTE_SIM_UNLOCK_ERROR;
                loge("deregisterCallback NullPointerException");
            }
            return mRsuResult;
        }

        @Override
        public int remoteSimlockProcessSimlockData(int token, byte[] data) {
            logi("remoteSimlockProcessSimlockData data = " + bytes2Hexs(data) + " token = "
                    + token);
            try {
                mRsuResult = ((MtkUiccController) MtkUiccController.getInstance()).getRsuSml()
                        .remoteSimlockProcessSimlockData(token, data);
            } catch (NullPointerException e) {
                mRsuResult = REMOTE_SIM_UNLOCK_ERROR;
                loge("remoteSimlockProcessSimlockData NullPointerException");
            }
            return mRsuResult;
        }

        @Override
        public int remoteSimlockGetVersion(int token) {
            logi("remoteSimlockGetVersion token = " + token);
            try {
                mRsuResult = ((MtkUiccController) MtkUiccController.getInstance()).getRsuSml()
                        .remoteSimlockGetVersion(token);
            } catch (NullPointerException e) {
                mRsuResult = REMOTE_SIM_UNLOCK_ERROR;
                loge("remoteSimlockGetVersion NullPointerException");
            }
            return mRsuResult;
        }

        @Override
        public int remoteSimlockGetSimlockStatus(int token) {
            logi("remoteSimlockGetSimlockStatus token = " + token);
            try {
                mRsuResult = ((MtkUiccController) MtkUiccController.getInstance()).getRsuSml()
                        .remoteSimlockGetSimlockStatus(token);
            } catch (NullPointerException e) {
                mRsuResult = REMOTE_SIM_UNLOCK_ERROR;
                loge("remoteSimlockGetSimlockStatus NullPointerException");
            }
            return mRsuResult;
        }

        @Override
        public int remoteSimlockGenerateRequest(int token, int requestType) {
            logi("remoteSimlockGenerateRequest token = " + token + " requestType = " + requestType);
            try {
                mRsuResult = ((MtkUiccController) MtkUiccController.getInstance()).getRsuSml()
                        .remoteSimlockGenerateRequest(token, requestType);
            } catch (NullPointerException e) {
                mRsuResult = REMOTE_SIM_UNLOCK_ERROR;
                loge("remoteSimlockGenerateRequest NullPointerException");
            }
            return mRsuResult;
        }

        @Override
        public int remoteSimlockUnlockTimer(int token, int requestType) {
            logi("remoteSimlockUnlockTimer token = " + token + " requestType = " + requestType);
            try {
                mRsuResult = ((MtkUiccController) MtkUiccController.getInstance()).getRsuSml()
                        .remoteSimlockUnlockTimer(token, requestType);
            } catch (NullPointerException e) {
                mRsuResult = REMOTE_SIM_UNLOCK_ERROR;
                loge("remoteSimlockUnlockTimer NullPointerException");
            }
            return mRsuResult;
        }

        private String bytes2Hexs(byte[] bytes) {
            return IccUtils.bytesToHexString(bytes);
        }
    };

    private void logi(String s) {
        Log.e(TAG, "[RSU-SIMLOCK] " + s);
    }

    private void loge(String s) {
        Log.e(TAG, "[RSU-SIMLOCK] " + s);
    }
}
