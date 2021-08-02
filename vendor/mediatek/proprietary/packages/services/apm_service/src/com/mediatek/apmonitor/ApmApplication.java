package com.mediatek.apmonitor;

import com.mediatek.apmonitor.monitor.*;

import android.app.Application;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.SystemProperties;
import android.util.Log;

import com.mediatek.common.carrierexpress.CarrierExpressManager;

import java.util.ArrayList;
import java.util.List;

public class ApmApplication extends Application {
    public static final String TAG = "APM-ServiceJ";

    private ApmHandler mHandler = null;
    private HandlerThread mHandlerThread = null;
    private List<KpiMonitor> mKpiMonitors = null;
    private ApmKpiReceiver mReceiver = null;
    protected ApmSession mApmSession = null;
    private Object mLock = new Object();
    private IApmSessionCallback mSessionCallback = null;
    private ArrayList<Short> mSubscribedMsgList = null;

    private boolean mInitialized = false;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, "ApmApplication onCreate()");
        if (ApmServiceUtils.isDmcSupport()) {
            initialize();
        } else {
            Log.w(TAG, "DMC not enabled, bye~");
        }
    }

    public void initialize() {
        mApmSession = new ApmSession();
        mSubscribedMsgList = new ArrayList<Short>();
        mSessionCallback = new ApmSessionCallback();
        mApmSession.registControlCallback(mSessionCallback);
        initHandlerThread();
        setupKpiMonitors();
    }

    public ApmSession getApmClient() {
        return mApmSession;
    }

    private void setupKpiMonitors() {
        synchronized(mLock) {
            if (mReceiver == null) {
                 mReceiver = new ApmKpiReceiver(this, mHandler);
            }

            if (mKpiMonitors == null) {
                mKpiMonitors = new ArrayList<KpiMonitor>();
                mKpiMonitors.add(new FooBarKpiMonitor(this, mHandler));
            }

            IntentFilter filter = new IntentFilter();
            filter.addAction(CarrierExpressManager.ACTION_OPERATOR_CONFIG_CHANGED);
            // To initialize APM and create HIDL sessino to APM server
            filter.addAction(IApmKpiMonitor.ACTION_APM_INIT_EVENT);
            // To register APM KPI Intent
            filter.addAction(IApmKpiMonitor.ACTION_APM_KPI_EVENT);
            Log.d(TAG, "start monitoring APM KPI, APM activated = " + ApmServiceUtils.isApmActive());
            this.registerReceiver(mReceiver, filter);
        }
    }

    private void initHandlerThread() {
        if (mInitialized) {
            return;
        }
        mHandlerThread = new HandlerThread("apm_srv");
        mHandlerThread.start();
        mHandler = new ApmHandler(this, mHandlerThread.getLooper());
        mInitialized = true;
        Log.d(TAG, "APM monitor thread initialized");
    }

    class ApmHandler extends Handler {
        private Context mContext = null;

        private ApmHandler() {}

        public ApmHandler(Context context, Looper looper) {
            super(looper);
            mContext = context;
        }

        @Override
        public void handleMessage(Message msg) {
            if (!ApmServiceUtils.isApmActive()) {
                return;
            }

            synchronized(mLock) {
                for (KpiMonitor monitor : mKpiMonitors) {
                    if (monitor != null) {
                        monitor.handleMessage(msg);
                    }
                }
            }
        }
    }

    class ApmSessionCallback implements IApmSessionCallback {
        @Override
        public void onQueryKpi(Short msgId) {
            Log.d(TAG, "onQueryKpi(" + msgId + ")");
            Message msg = mHandler.obtainMessage(
                        KpiMonitor.MSG_ID_APM_KPI_ON_QUERY, msgId, 0);
            mHandler.sendMessage(msg);
        }

        @Override
        public void onKpiSubscriptionStateChanged(ArrayList<Short> subscribedMessageList) {
            // Check the difference and notify KpiMonitors
            List<Short> removedList = new ArrayList<Short>(mSubscribedMsgList);
            List<Short> addedList = new ArrayList<Short>(subscribedMessageList);
            removedList.removeAll(subscribedMessageList);
            addedList.removeAll(mSubscribedMsgList);

            for (Short msgId : removedList) {
                Message msg = mHandler.obtainMessage(
                        KpiMonitor.MSG_ID_APM_KPI_STATE_CHANGED,
                        KpiMonitor.EVENT_KPI_REMOVED, 0, (Object)msgId);
                mHandler.sendMessage(msg);
                Log.d(TAG, "Remove APM msgId = " + msgId);
            }
            for (Short msgId : addedList) {
                Message msg = mHandler.obtainMessage(
                        KpiMonitor.MSG_ID_APM_KPI_STATE_CHANGED,
                        KpiMonitor.EVENT_KPI_ADDED, 0, (Object)msgId);
                mHandler.sendMessage(msg);
                Log.d(TAG, "Add APM msgId = " + msgId);
            }
            // Update cache
            mSubscribedMsgList.clear();
            for (Short msgId : subscribedMessageList) {
                mSubscribedMsgList.add(msgId);
            }
        }
    }
}
