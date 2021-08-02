/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

package com.mediatek.engineermode.dm;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.FeatureSupport;
import com.mediatek.engineermode.R;

import java.lang.StringBuilder;

/**
 * Activity for diagnostic monitoring settings.
 *
 */
public class DiagnosticMonitoringSettingActivity extends Activity  {

    private TextView mTvDmcFo = null;
    private TextView mTvMapiFo = null;
    private TextView mTvMdmiFo = null;
    private Button mBtnLogSetting = null;
    private Button mBtnLogPkmSetting = null;
    private Button mBtnApmSetting = null;

    private DmSettingController mDmSettingCtrl = null;
    private static final int DLG_DM_LOG_SETTING_SWITCHING = 0;
    private static final int DLG_DM_LOG_PKM_SETTING_SWITCHING = 1;
    private static final int DLG_DM_APM_SETTING_SWITCHING = 2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.diagnostic_monitoring_setting);

        mTvDmcFo = (TextView) findViewById(R.id.dm_fo_dmc_text);
        mTvMapiFo = (TextView) findViewById(R.id.dm_fo_mapi_text);
        mTvMdmiFo = (TextView) findViewById(R.id.dm_fo_mdmi_text);

        mBtnLogSetting = (Button) findViewById(R.id.dm_log_setting_button);
        mBtnLogPkmSetting = (Button) findViewById(R.id.dm_log_pkm_setting_button);
        mBtnApmSetting = (Button) findViewById(R.id.dm_apm_setting_button);

        mDmSettingCtrl = new DmSettingController(this);
        updateUI();

        mBtnLogSetting.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                showDialog(DLG_DM_LOG_SETTING_SWITCHING);
                LogSettingTask task = new LogSettingTask();
                task.execute();
           }
        });
        mBtnLogPkmSetting.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                showDialog(DLG_DM_LOG_PKM_SETTING_SWITCHING);
                LogPkmSettingTask task = new LogPkmSettingTask();
                task.execute();
           }
        });
        mBtnApmSetting.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                showDialog(DLG_DM_APM_SETTING_SWITCHING);
                ApmSettingTask task = new ApmSettingTask();
                task.execute();
           }
        });
    }

    private void updateUI() {
        StringBuilder dmcFoString = new StringBuilder();
        StringBuilder mapiFoString = new StringBuilder();
        StringBuilder mdmiFoString = new StringBuilder();

        // DMC FO
        dmcFoString.append("DMC: system(" + DmSettingController.isDmcSystemEnabled() + ") ");
        dmcFoString.append("vendor(" + DmSettingController.isDmcVendorEnabled() + ")");
        mTvDmcFo.setText(dmcFoString.toString());
        // MAPI FO
        mapiFoString.append("MAPI: system(" + DmSettingController.isMapiSystemEnabled() + ") ");
        mapiFoString.append("vendor(" + DmSettingController.isMapiVendorEnabled() + ")");
        mTvMapiFo.setText(mapiFoString.toString());
        // MDMI FO
        mdmiFoString.append("MDMI: system(" + DmSettingController.isMdmiSystemEnabled() + ") ");
        mdmiFoString.append("vendor(" + DmSettingController.isMdmiVendorEnabled() + ")");
        mTvMdmiFo.setText(mdmiFoString.toString());

        // Log Button
        if (mDmSettingCtrl.isDmLogEnabled()) {
            mBtnLogSetting.setText(R.string.dm_log_button_disable);
        } else {
            mBtnLogSetting.setText(R.string.dm_log_button_enable);
        }
        // Log PKM Button
        if (mDmSettingCtrl.isPkmLogEnabled()) {
            mBtnLogPkmSetting.setText(R.string.dm_log_pkm_button_disable);
        } else {
            mBtnLogPkmSetting.setText(R.string.dm_log_pkm_button_enable);
        }
        // APM
        if (mDmSettingCtrl.isDmcApmActivated()) {
            mBtnApmSetting.setText(R.string.dm_apm_button_disable);
        } else {
            mBtnApmSetting.setText(R.string.dm_apm_button_enable);
        }
    }

    /**
     * Task to switching.
     */
    private class LogSettingTask extends AsyncTask<Void, Void, Boolean> {
        @Override
        protected Boolean doInBackground(Void... params) {
            Boolean enabled = mDmSettingCtrl.isDmLogEnabled();
            return mDmSettingCtrl.enableDmLog(!enabled);
        }

        @Override
        protected void onPostExecute(Boolean result) {
            updateUI();
            removeDialog(DLG_DM_LOG_SETTING_SWITCHING);
            if (isActivityAlive()) {
                AlertDialog.Builder builder =
                        new AlertDialog.Builder(DiagnosticMonitoringSettingActivity.this);
                builder.setTitle(
                        result ? R.string.set_success_message : R.string.set_fail_message).show();
            }
        }
    }

    private class LogPkmSettingTask extends AsyncTask<Void, Void, Boolean> {
        @Override
        protected Boolean doInBackground(Void... params) {
            Boolean enabled = mDmSettingCtrl.isPkmLogEnabled();
            return mDmSettingCtrl.enablePkmLog(!enabled);
        }

        @Override
        protected void onPostExecute(Boolean result) {
            updateUI();
            removeDialog(DLG_DM_LOG_PKM_SETTING_SWITCHING);
            if (isActivityAlive()) {
                AlertDialog.Builder builder =
                        new AlertDialog.Builder(DiagnosticMonitoringSettingActivity.this);
                builder.setTitle(
                        result ? R.string.set_success_message : R.string.set_fail_message).show();
            }
        }
    }

    private class ApmSettingTask extends AsyncTask<Void, Void, Boolean> {
        @Override
        protected Boolean doInBackground(Void... params) {
            Boolean activated = mDmSettingCtrl.isDmcApmActivated();
            return mDmSettingCtrl.activeApm(!activated);
        }

        @Override
        protected void onPostExecute(Boolean result) {
            updateUI();
            removeDialog(DLG_DM_APM_SETTING_SWITCHING);
            if (isActivityAlive()) {
                AlertDialog.Builder builder =
                        new AlertDialog.Builder(DiagnosticMonitoringSettingActivity.this);
                builder.setTitle(
                        result ? R.string.set_success_message : R.string.set_fail_message).show();
            }
        }
    }

    @Override
    protected Dialog onCreateDialog(int id) {
        ProgressDialog dialog = null;
        switch (id) {
            case DLG_DM_LOG_SETTING_SWITCHING:
            case DLG_DM_LOG_PKM_SETTING_SWITCHING:
            case DLG_DM_APM_SETTING_SWITCHING:
                dialog = new ProgressDialog(
                        DiagnosticMonitoringSettingActivity.this);
                dialog.setMessage(getString(R.string.dm_executing));
                dialog.setCancelable(false);
                dialog.setIndeterminate(true);
                break;
            default:
                break;
        }
        return dialog;
    }

    /**
     * Function to indicate if the DM settings is supported.
     * @return true for support
     */
    public static boolean isSupport() {
        if (DmSettingController.isDmcVendorEnabled()) {
            return true;
        }
        return false;
    }

    private boolean isActivityAlive() {
        boolean isFinishing = isFinishing();
        boolean isDestroyed = isDestroyed();
        //Elog.d(TelLogController.TAG, "isFinishing: " + isFinishing
        //       + " isDestroyed: " + isDestroyed);
        return (!isFinishing) && (!isDestroyed);
    }
}
