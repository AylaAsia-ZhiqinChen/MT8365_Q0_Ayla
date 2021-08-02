package com.android.imeisettings;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.res.Resources;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.view.MenuItem;
import android.content.ContentResolver;
import android.provider.Settings;
//yuntian longyao add
//Description:非机主模式下禁止设置imei号
import android.os.UserHandle;
//yuntian longyao end

public class ImeiSettings extends Activity implements View.OnClickListener {

    private TextView mSim1ImeiTv;
    private TextView mSim2ImeiTv;
    private EditText mSim1ImeiEt;
    private EditText mSim2ImeiEt;
    private Button mSettingBt;
    private Button mClearNvBt;
    private SettingHelper mHelper;
    private Dialog mSettingDialog;

    private String mSim1Imei;
    private String mSim2Imei;
    private int mSettingCount;
    private int mSuccessCount;
    private int mFailCount;
    private boolean mEnabledSettingSim1Imei;
    private boolean mEnabledSettingSim2Imei;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getActionBar().setDisplayHomeAsUpEnabled(true);
        setContentView(R.layout.activity_imei_settings);

        Resources res = getResources();
        mEnabledSettingSim1Imei = res.getBoolean(R.bool.enabled_setting_sim1_imei);
        mEnabledSettingSim2Imei = res.getBoolean(R.bool.enabled_setting_sim2_imei);
        mHelper = new SettingHelper(this, mHandler);
        mSettingDialog = createSettingProgressDialog();

        mSim1ImeiTv = (TextView) findViewById(R.id.sim1_imei_title);
        mSim2ImeiTv = (TextView) findViewById(R.id.sim2_imei_title);
        mSim1ImeiEt = (EditText) findViewById(R.id.sim1_imei);
        mSim2ImeiEt = (EditText) findViewById(R.id.sim2_imei);
        mSettingBt = (Button) findViewById(R.id.setting);
        mClearNvBt = (Button) findViewById(R.id.clear);
        mSettingBt.setOnClickListener(this);
        mClearNvBt.setOnClickListener(this);
        int dfkj = R.string.app_name;
    }

    @Override
    protected void onStart() {
        super.onStart();
        updateViews();
    }
    
    @Override
    public boolean onMenuItemSelected(int featureId, MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                finish();
                return true;
        }
        return super.onMenuItemSelected(featureId, item);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.setting:
                mSim1Imei = mSim1ImeiEt.getText().toString().trim();
                mSim2Imei = mSim2ImeiEt.getText().toString().trim();
                boolean enabledSetting = mHelper.enabledSettingImei(mSim1Imei, mSim2Imei, true);
                if (enabledSetting) {
                    mSuccessCount = 0;
                    mFailCount = 0;
                    mSettingCount = mHelper.getSettingCount(mSim1Imei, mSim2Imei);
                    if (mSettingCount > 0) {
                        //yuntian longyao add
                        //Description:非机主模式下禁止设置imei号
                        if (UserHandle.myUserId() == UserHandle.USER_OWNER) {
                            if (!mSettingDialog.isShowing()) {
                                mSettingDialog.show();
                            }
                        }else {
                            finish();
                        }
						//yuntian longyao end
                        mSim1ImeiEt.setEnabled(false);
                        mSim2ImeiEt.setEnabled(false);
                        mSettingBt.setEnabled(false);
                        mClearNvBt.setEnabled(false);
                        mHandler.sendEmptyMessage(SettingHelper.MSG_SETTING_SIM1_IMEI);
                    }
                }
                break;

            case R.id.clear:
                if (mHelper.clearNV()) {
                    Toast.makeText(this, R.string.clear_success, Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(this, R.string.clear_fail, Toast.LENGTH_SHORT).show();
                }
                break;
        }
    }

    private void updateViews() {
        mSim1ImeiTv.setVisibility(mEnabledSettingSim1Imei ? View.VISIBLE : View.GONE);
        mSim1ImeiEt.setVisibility(mEnabledSettingSim1Imei ? View.VISIBLE : View.GONE);
        mSim2ImeiTv.setVisibility(mEnabledSettingSim2Imei ? View.VISIBLE : View.GONE);
        mSim2ImeiEt.setVisibility(mEnabledSettingSim2Imei ? View.VISIBLE : View.GONE);

        if (mEnabledSettingSim1Imei) {
            String sim1Imei = mHelper.getDeviceId(0);
            if (!TextUtils.isEmpty(sim1Imei)) {
                mSim1ImeiEt.setHint(sim1Imei);
            }
        }
        if (mEnabledSettingSim2Imei) {
            String sim2Imei = mHelper.getDeviceId(1);
            if (!TextUtils.isEmpty(sim2Imei)) {
                mSim2ImeiEt.setHint(sim2Imei);
            }
        }
    }

    private Dialog createSettingProgressDialog() {
        ProgressDialog dialog = new ProgressDialog(this, R.style.ProgressDialogTheme);
        dialog.setMessage(getString(R.string.setting_dialog_msg));
        dialog.setCanceledOnTouchOutside(false);
        return dialog;
    }

    private void needShowResultDialog() {
        Log.d(ImeiSettings.this, "needShowResultDialog=>count: " + mSettingCount + " success: " + mSuccessCount + " fail: " + mFailCount);
        if (mSettingDialog.isShowing()) {
            try {
                mSettingDialog.dismiss();
            } catch (Exception e) {
                Log.e(this, "needShowResultDialog=>error: " , e);
            }
        }
        if (mSuccessCount + mFailCount == mSettingCount) {
            AlertDialog.Builder builder = new AlertDialog.Builder(ImeiSettings.this);
            builder.setTitle(R.string.setting_imei);
            builder.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    if (mSuccessCount == mSettingCount) {
                        ImeiSettings.this.finish();
                    }
                }
            });
            builder.setCancelable(false);
            if (mSuccessCount == mSettingCount) {
				builder.setMessage(R.string.success);
            } else {
				builder.setMessage(R.string.fail);
            }
            builder.create().show();
            mSim1ImeiEt.setEnabled(true);
            mSim2ImeiEt.setEnabled(true);
            mSettingBt.setEnabled(true);
            mClearNvBt.setEnabled(true);
        }
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            Log.d(ImeiSettings.this, "handleMessage=>what: " + msg.what);
            String sim1Imei = mSim1Imei;
            String sim2Imei = mSim2Imei;
            if (mHelper.needSwapImei()) {
                sim1Imei = mSim2Imei;
                sim2Imei = mSim1Imei;
            }
            switch (msg.what) {
                case SettingHelper.MSG_SETTING_SIM1_IMEI:
                    if (mHelper.needSettingSim1Imei(sim1Imei)) {
                        mHelper.executeWriteSim1ImeiATCommand(sim1Imei);
                    } else if (mHelper.needSettingSim2Imei(sim2Imei)) {
                        mHelper.executeWriteSim2ImeiATCommand(sim2Imei);
                    }
                    break;

                case SettingHelper.MSG_SETTING_SIM2_IMEI:
                    if (mHelper.needSettingSim2Imei(sim2Imei)) {
                        mHelper.executeWriteSim2ImeiATCommand(sim2Imei);
                    } else {
                        if (mHelper.needSettingSim1Imei(sim1Imei) || mHelper.needSettingSim2Imei(sim2Imei)) {
                            mHandler.sendEmptyMessage(SettingHelper.MSG_RESTART_GSM_MODE);
                        }
                    }
                    break;

                case SettingHelper.MSG_RESTART_GSM_MODE:
                    mHelper.restartGsmMode();
                    break;

                case SettingHelper.MSG_SETTING_SIM1_IMEI_RESULT:
                    Log.d(ImeiSettings.this, "handleMessage=>MSG_SETTING_SIM1_IMEI_RESULT: " + ar.exception);
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
                    } else {
                        mFailCount++;
                    }
                    mHandler.sendEmptyMessage(SettingHelper.MSG_SETTING_SIM2_IMEI);
                    break;

                case SettingHelper.MSG_SETTING_SIM2_IMEI_RESULT:
                    Log.d(ImeiSettings.this, "handleMessage=>MSG_SETTING_SIM2_IMEI_RESULT: " + ar.exception);
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
					Log.d(ImeiSettings.this, "handleMessage(MSG_RESTART_GSM_MODE_RESULT)=>count: " 
										+ mSettingCount + " success: " + mSuccessCount + " fail: " + mFailCount);
                    Log.d(ImeiSettings.this, "handleMessage=>MSG_RESTART_GSM_MODE_RESULT: " + ar.exception);
                    if (ar.exception != null) {
                        mFailCount = mSettingCount;
                        mSuccessCount = 0;
                    }
					if (mSettingCount > 0) {
						mHandler.sendEmptyMessageDelayed(SettingHelper.MSG_NOTIFY_IMEI_CHANGED, 
								SettingHelper.NOTIFY_IMEI_CHANGED_DELAYED);
					} else {
						needShowResultDialog();
					}
                    break;
                    
				case SettingHelper.MSG_NOTIFY_IMEI_CHANGED:
					mHelper.sendImeiChangedBroadcast();
					needShowResultDialog();
					break;
			}
        }
    };
}
