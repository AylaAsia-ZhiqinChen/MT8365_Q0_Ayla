package com.android.imeisettings;

import android.app.Service;
import android.content.Intent;
import android.content.res.Resources;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.SystemProperties;
import android.text.TextUtils;
import android.content.ContentResolver;
import android.provider.Settings;
import android.app.NotificationManager;
import android.app.NotificationChannel;
import android.content.Context;
import android.app.Notification;

public class UpdateService extends Service {

    public static final String EXTRA_ACTION = "action";

    private static final int MSG_UPDATE_IMEI = 100;

    private SettingHelper mHelper;

    private String mSim1Imei;
    private String mSim2Imei;
    private long mUpdateImeiDelayedTime;
    private int mSettingCount;
    private int mSuccessCount;
    private int mFailCount;
    private boolean mEnabledSettingSim1Imei;
    private boolean mEnabledSettingSim2Imei;

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(this, "onCreate()...");
        Resources res = getResources();
        mUpdateImeiDelayedTime = res.getInteger(R.integer.update_imei_delayed_time);
        mEnabledSettingSim1Imei = res.getBoolean(R.bool.enabled_setting_sim1_imei);
        mEnabledSettingSim2Imei = res.getBoolean(R.bool.enabled_setting_sim2_imei);

        mHelper = new SettingHelper(this, mHandler);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(this, "onStartCommand=>intent: " + intent + " flags: " + flags + " startId: " + startId);
        NotificationManager mgr = (NotificationManager)this.
          getSystemService(Context.NOTIFICATION_SERVICE);
        mgr.createNotificationChannel(new NotificationChannel( "imei_notification_channel", "imeisetting", NotificationManager.IMPORTANCE_LOW));
         Notification.Builder builder = new Notification.Builder(getApplicationContext(),"imei_notification_channel"); //与channelId对应
        Notification notification = builder.build();
        startForeground(1, notification);
        if (intent != null && Intent.ACTION_BOOT_COMPLETED.equals(intent.getStringExtra(EXTRA_ACTION))) {
            if (mHandler.hasMessages(MSG_UPDATE_IMEI)) {
                mHandler.removeMessages(MSG_UPDATE_IMEI);
            }
            mHandler.sendEmptyMessageDelayed(MSG_UPDATE_IMEI, mUpdateImeiDelayedTime);
        } else {
            if (!mHandler.hasMessages(MSG_UPDATE_IMEI)) {
                stopSelf();
            }
        }
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(this, "onDestroy()...");
    }

    private void updateImei() {
        mSim1Imei = mHelper.getSim1ImeiFromNv();
        mSim2Imei = mHelper.getSim2ImeiFromNv();
        Log.d(this, "updateImei(nv)=>imei1: " + mSim1Imei + " imei2: " + mSim2Imei);

        String sim1Imei = mHelper.getDeviceId(0);
        String sim2Imei = mHelper.getDeviceId(1);
        Log.d(this, "updateImei(device)=>imei1: " + sim1Imei + " imei2: " + sim2Imei);

        if (TextUtils.isEmpty(mSim1Imei) || mSim1Imei.equals(sim1Imei)) {
            mSim1Imei = "";
        }
        if (TextUtils.isEmpty(mSim2Imei) || mSim2Imei.equals(sim2Imei)) {
            mSim2Imei = "";
        }
        
        Log.d(this, "updateImei(final)=>imei1: " + mSim1Imei + " imei2: " + mSim2Imei);
        boolean enabledSetting = mHelper.enabledSettingImei(mSim1Imei, mSim2Imei, false);
        if (enabledSetting) {
            mSuccessCount = 0;
            mFailCount = 0;
            mSettingCount = mHelper.getSettingCount(mSim1Imei, mSim2Imei);
            if (mSettingCount > 0) {
                mHandler.sendEmptyMessage(SettingHelper.MSG_SETTING_SIM1_IMEI);
            } else {
                stopSelf();
            }
        } else {
            stopSelf();
        }
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Log.d(UpdateService.this, "handleMessage=>what: " + msg.what);
            AsyncResult ar = (AsyncResult) msg.obj;
            String sim1Imei = mSim1Imei;
            String sim2Imei = mSim2Imei;
            if (mHelper.needSwapImei()) {
                sim1Imei = mSim2Imei;
                sim2Imei = mSim1Imei;
            }
            switch (msg.what) {
                case MSG_UPDATE_IMEI:
                    updateImei();
                    break;

                case SettingHelper.MSG_SETTING_SIM1_IMEI:
                    if (mHelper.needSettingSim1Imei(sim1Imei)) {
                        mHelper.executeWriteSim1ImeiATCommand(sim1Imei);
                    } else if (mHelper.needSettingSim2Imei(sim2Imei)) {
                        mHelper.executeWriteSim2ImeiATCommand(sim2Imei);
                    } else {
                        stopSelf();
                    }
                    break;

                case SettingHelper.MSG_SETTING_SIM2_IMEI:
                    if (mHelper.needSettingSim2Imei(sim2Imei)) {
                        mHelper.executeWriteSim2ImeiATCommand(sim2Imei);
                    } else {
                        if (mHelper.needSettingSim1Imei(sim1Imei) || mHelper.needSettingSim2Imei(sim2Imei)) {
                            mHandler.sendEmptyMessage(SettingHelper.MSG_RESTART_GSM_MODE);
                        } else {
                            stopSelf();
                        }
                    }
                    break;

                case SettingHelper.MSG_RESTART_GSM_MODE:
                    mHelper.restartGsmMode();
                    break;

                case SettingHelper.MSG_SETTING_SIM1_IMEI_RESULT:
                    Log.d(UpdateService.this, "handleMessage=>MSG_SETTING_SIM1_IMEI_RESULT: " + ar.exception);
                    if (ar.exception == null) {
                        boolean result = false;
                        if (mHelper.needSwapImei()) {
                            result = mHelper.writeSim2ImeiToNv(mSim2Imei.trim());
                        } else {
                            result = mHelper.writeSim1ImeiToNv(mSim1Imei.trim());
                        }
                        if (result) {
                            mSuccessCount++;
                        } else {
                            mFailCount++;
                        }
                        mHandler.sendEmptyMessage(SettingHelper.MSG_SETTING_SIM2_IMEI);
                    } else {
                        mFailCount++;
                        mHandler.sendEmptyMessage(SettingHelper.MSG_SETTING_SIM2_IMEI);
                    }
                    break;

                case SettingHelper.MSG_SETTING_SIM2_IMEI_RESULT:
                    Log.d(UpdateService.this, "handleMessage=>MSG_SETTING_SIM2_IMEI_RESULT: " + ar.exception);
                    if (ar.exception == null) {
                        boolean result = false;
                        if (mHelper.needSwapImei()) {
                            result = mHelper.writeSim1ImeiToNv(mSim1Imei.trim());
                        } else {
                            result = mHelper.writeSim2ImeiToNv(mSim2Imei.trim());
                        }
                        if (result) {
                            mSuccessCount++;
                        } else {
                            mFailCount++;
                        }
                    } else {
                        mFailCount++;
                    }
                    mHandler.sendEmptyMessage(SettingHelper.MSG_RESTART_GSM_MODE);
                    break;

                case SettingHelper.MSG_RESTART_GSM_MODE_RESULT:
					Log.d(UpdateService.this, "handleMessage(MSG_RESTART_GSM_MODE_RESULT)=>count: " 
										+ mSettingCount + " success: " + mSuccessCount + " fail: " + mFailCount);
                    Log.d(UpdateService.this, "handleMessage=>MSG_RESTART_GSM_MODE_RESULT: " + ar.exception);
                    if (ar.exception != null) {
                        mFailCount = mSettingCount;
                        mSuccessCount = 0;
                    }
                    if (mSettingCount > 0) {
						mHandler.sendEmptyMessageDelayed(SettingHelper.MSG_NOTIFY_IMEI_CHANGED, 
								SettingHelper.NOTIFY_IMEI_CHANGED_DELAYED);
					} else {
						stopSelf();
					}
                    break;
                    
				case SettingHelper.MSG_NOTIFY_IMEI_CHANGED:
					mHelper.sendImeiChangedBroadcast();
					stopSelf();
					break;
            }
        }
    };
}
