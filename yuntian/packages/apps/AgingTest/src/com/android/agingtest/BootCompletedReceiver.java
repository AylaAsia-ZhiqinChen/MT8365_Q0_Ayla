package com.android.agingtest;

import java.util.ArrayList;

import com.android.agingtest.test.RebootActivity;
import com.android.agingtest.test.ResetActivity;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.Resources;
import android.os.RemoteException;
import android.os.SystemClock;
import android.preference.PreferenceManager;
import android.os.PowerManager;

import com.android.internal.telephony.TelephonyIntents;

import android.net.Uri;

import com.mediatek.nvram.NvRAMUtils;

public class BootCompletedReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d(this, "onReceive=>action: " + intent.getAction());
        String action = intent.getAction();
        Resources res = context.getResources();
        PowerManager pm = ((PowerManager) context.getSystemService(Context.POWER_SERVICE));
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(context);
        if (Intent.ACTION_BOOT_COMPLETED.equals(action)) {
//			boolean isCirculation = sp.getBoolean(TestUtils.CIRCULATION_STATE,
//					res.getBoolean(R.bool.default_circulation_value));
            byte resetTime = 0;
            try {
                //resetTime = NvRAMUtils.readNV(NvRAMUtils.INDEX_RESET_CURRENT_TIME);
                byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_RESET_CURRENT_TIME, NvRAMUtils.RESET_CURRENT_TIME_LENGTH);
                if (buff != null) {
                    resetTime = buff[0];
                }
            } catch (RemoteException e) {
                // TODO: handle exception
            }
            android.util.Log.e("lsz", "onReceive-resetTime->" + resetTime);
            if (resetTime > 0) {
                Intent i = new Intent();
                i.setClass(context, ResetActivity.class);
                i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
//				i1.setComponent(new ComponentName("com.android.agingtest", "com.android.agingtest.ResetActivity"));
                context.startActivity(i);
                android.util.Log.e("lsz", "onReceive-startActivity->");
                return;
            }
            boolean isAgingTest = sp.getBoolean(TestUtils.IS_TEST, false);
            Log.e("lsz", "isAgingTest->" + isAgingTest);
            if (!isAgingTest) {
                return;
            }
            int mRebootTimes = sp.getInt(TestUtils.REBOOT_KEY + TestUtils.TEST_TIME, 20);
            if (context.getResources().getBoolean(R.bool.use_total_test_time)) {
                mRebootTimes = sp.getInt("singleTestTime", 20);
            }
            int counter = sp.getInt(TestUtils.CURRENT_REBOOT_TIMES, -1);
            Log.d("lsz", "mRebootTimes: " + mRebootTimes + ",counter->" + counter);
            //只要时在测试开关机，counter必会大于０，反之不是在进行开关机测试。
            if (counter > 0) {
                if (!pm.isScreenOn()) {
                    pm.wakeUp(SystemClock.uptimeMillis());
                }
                int length = TestUtils.ALLKEYS.length;
                int[] allKeyIndexs = new int[length];
                int j = 0;
//				算出需要测试的item个数
                for (int i = 0; i < length; i++) {
                    if (sp.getBoolean(TestUtils.ALLKEYS[i], false)) {
                        Log.e(this, "onReceive=>i-lsz-->" + i);
                        j++;
                    }
                }
                int[] alltestKeyIndexs = new int[j];
                j = 0;
//				记录需要测试的item的key对应的index
                for (int i = 0; i < length; i++) {
                    if (sp.getBoolean(TestUtils.ALLKEYS[i], false)) {
                        Log.e(this, "onReceive=>i-lsz-->" + i);
                        alltestKeyIndexs[j++] = i;
                    }
                }
                int currentIndex = 0;

                if (counter >= mRebootTimes) {
                    Log.e(this, "onReceive=>i-alltestKeyIndexs-->" + alltestKeyIndexs.length);
                    Editor e = sp.edit();
                    e.putInt(TestUtils.REBOOT_KEY + TestUtils.TEST_RESULT, 1);
                    try {
                        byte result = -2;
                        byte[] buff = new byte[1];
                        buff[0] = result;
                        NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_REBOOT, buff);
                    } catch (RemoteException e1) {
                    }
//					e.putBoolean(TestUtils.TEST_STATE, false);
                    e.commit();
                    if (currentIndex + 1 < alltestKeyIndexs.length) {
                        Intent nextIntent = new Intent(context, TestUtils.ALLCLASSES[alltestKeyIndexs[currentIndex]]);
                        nextIntent.putExtra(TestUtils.ALLTESTKEYINDEX, alltestKeyIndexs);
                        nextIntent.putExtra(TestUtils.KEY_INDEX, currentIndex);
                        nextIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
                        context.startActivity(nextIntent);
                    } else {
//						Editor editor = sp.edit();
//						editor.putBoolean(TestUtils.TEST_STATE, false);
//						editor.commit();
                        Intent report = new Intent(context,
                                ReportActivity.class);
//						intent.putExtra(TestUtils.CIRCULATION_EXTRA, true);
                        report.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK
                                | Intent.FLAG_ACTIVITY_CLEAR_TOP
                                | Intent.FLAG_ACTIVITY_NEW_TASK);
                        context.startActivity(report);
                    }
                } else {
                    Log.e(this, "onReceive=>i-RebootActivity-->");
                    Intent reboot = new Intent(context, RebootActivity.class);

                    reboot.putExtra(TestUtils.ALLTESTKEYINDEX, alltestKeyIndexs);
                    reboot.putExtra(TestUtils.KEY_INDEX, currentIndex);
                    reboot.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK
                            | Intent.FLAG_ACTIVITY_CLEAR_TOP
                            | Intent.FLAG_ACTIVITY_NEW_TASK);
                    context.startActivity(reboot);
                }
            }

        } else if (TelephonyIntents.SECRET_CODE_ACTION.equals(action)) {
            Uri agingTestUri = Uri.parse("android_secret_code://" + context.getResources().getString(R.string.aging_test_secret_code));
            Uri uri = intent.getData();
            Log.d(this, "onReceive=>uri: " + uri + " agingTest: " + agingTestUri);
            if (agingTestUri.equals(uri)) {
                Intent agingTest = new Intent(context, AgingTestMainActivity.class);
                agingTest.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
                context.startActivity(agingTest);
            }
        }
    }

}
