package com.android.agingtest.test;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.TimeZone;

import com.android.agingtest.AgingTestInterFace;
import com.android.agingtest.BaseActivity;
import com.android.agingtest.Log;
import com.android.agingtest.TestUtils;
import com.android.agingtest.R;
import com.android.agingtest.ReportActivity;

import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.os.RemoteException;
import android.os.PowerManager.WakeLock;
import android.os.SystemClock;
import android.preference.PreferenceManager;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.mediatek.nvram.NvRAMUtils;


public class ResetActivity extends Activity {

    private final static String TAG = "lszResetActivity";
    public static final int FLAG_HOMEKEY_DISPATCHED = 0x80000000;

    private static final int MSG_UPDATE_TIME = 0;
    private static final int MSG_REBOOT = 1;

    private Button mStopBt;

    private int mResetDelayed;

    private TextView timeTextView;
    private Button stopBtn;


    private byte currentTime = -1;
    private byte resetCount = -1;

    public PowerManager.WakeLock mLock;
    public PowerManager mPowerManager;
    private SharedPreferences mSharedPreferences;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD | WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.base_layout);
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        timeTextView = (TextView) findViewById(R.id.test_time);
        stopBtn = (Button) findViewById(R.id.stop_test);
        stopBtn.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                stopTest(false);
            }
        });
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_RESET_CURRENT_TIME, NvRAMUtils.RESET_CURRENT_TIME_LENGTH);
            byte[] buff1 = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_RESET_ALL_TIMES, NvRAMUtils.RESET_ALL_TIMES_LENGTH);
            currentTime = buff[0];
            resetCount = buff[0];
        } catch (RemoteException e) {
            // TODO: handle exception
        }

        Log.e("lsz", " onCreate currentTime->" + currentTime + "resetTime->" + resetCount);
        if (currentTime <= 0) {
            //showDialog();
            try {
                byte[] buff = new byte[1];
                buff[0] = 1;
                NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_RESET_ALL_TIMES, buff);
            } catch (RemoteException e) {
                // TODO: handle exception
            }
            doTest();
        } else {
            doTest();
        }
        mPowerManager = ((PowerManager) getSystemService(Context.POWER_SERVICE));
        mLock = mPowerManager.newWakeLock(1, "sprocomm");
        mPowerManager.wakeUp(SystemClock.uptimeMillis());
    }


    @Override
    protected void onResume() {
        // TODO Auto-generated method stub
        super.onResume();
        mLock.acquire();
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
        if (mLock.isHeld()) {
            mLock.release();
        }
    }

    private void showDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(R.string.set_times);
        View v = getLayoutInflater().inflate(R.layout.set_edit, null);
        final EditText et = (EditText) v.findViewById(R.id.set_edit);
        builder.setView(v);
        builder.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                // TODO Auto-generated method stub
                //存入测试次数
                if (!et.getText().toString().equals("")) {
                    int temp = Integer.parseInt(et.getText().toString());
                    if (temp > 127) {
                        Toast.makeText(ResetActivity.this, getString(R.string.toolarge_tip), Toast.LENGTH_LONG).show();
                        finish();
                        return;
                    } else {
                        resetCount = Byte.parseByte(et.getText().toString());
                    }
                    boolean isSuccess = false;
                    try {
                        byte[] buff = new byte[1];
                        buff[0] = resetCount;
                        isSuccess = NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_RESET_ALL_TIMES, buff);
                    } catch (RemoteException e) {
                        // TODO: handle exception
                    }

                    if (!isSuccess) {
                        Log.e(TAG, "write NVRAM error");
                        finish();
                    } else {
                        try {
                            Log.e(TAG, "write NVRAM success resetCount->" + NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_RESET_ALL_TIMES, NvRAMUtils.RESET_ALL_TIMES_LENGTH));
                        } catch (RemoteException e) {
                            // TODO: handle exception
                        }
                    }
                    doTest();
                } else {
                    finish();
                }
            }
        }).setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                // TODO Auto-generated method stub
                finish();
            }
        });
        builder.setCancelable(false);
        builder.create().show();
    }


    public void doTest() {
        // TODO Auto-generated method stub
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_RESET_CURRENT_TIME, NvRAMUtils.RESET_CURRENT_TIME_LENGTH);
            currentTime = buff[0];
        } catch (RemoteException e) {
            // TODO: handle exception
        }

//		开始测试时currentTime必然小于０故初始为０
        if (currentTime < 0) {
            currentTime = 0;
        }
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_RESET_ALL_TIMES, NvRAMUtils.RESET_ALL_TIMES_LENGTH);
            resetCount = buff[0];
        } catch (RemoteException e) {
            // TODO: handle exception
        }


        Log.e("lsz", "doTest currentTime->" + currentTime + ",resetTime->" + resetCount);
        currentTime++;
        if (currentTime > resetCount) {
            stopTest(true);
            return;
        }
        timeTextView.setText(currentTime + " / " + resetCount);
        boolean isSuccess = false;

        try {
            byte[] buff = new byte[1];
            buff[0] = currentTime;
            isSuccess = NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_RESET_CURRENT_TIME, buff);
        } catch (RemoteException e) {
            // TODO: handle exception
        }

        Log.e("lsz", "doTest currentTime->" + currentTime + "resetCount->" + resetCount);
        //延时太短时开机便恢复出厂设置时会失败，此处开机后延时１分钟在进行恢复出厂设置。
        mHandler.sendEmptyMessageDelayed(RESET, 60 * 1000);
        if (!isSuccess) {
            stopTest(false);
        }
    }

    private static final int RESET = 0;
    private Handler mHandler = new Handler() {
        public void handleMessage(android.os.Message msg) {
            if (RESET == msg.what) {
                Log.e("lsz", "handleMessage-sendBroadcast-MASTER_CLEAR>");
                        /*
		        Intent intent = new Intent(Intent.ACTION_MASTER_CLEAR);
		        intent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND);
		        intent.putExtra(Intent.EXTRA_REASON, "MasterClearConfirm");
		        intent.putExtra(Intent.EXTRA_WIPE_EXTERNAL_STORAGE, false);
                        */

                Intent intent = new Intent(Intent.ACTION_FACTORY_RESET);
                intent.setPackage("android");
                intent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND);
                intent.putExtra(Intent.EXTRA_REASON, "MasterClearConfirm");
                intent.putExtra(Intent.EXTRA_WIPE_EXTERNAL_STORAGE, false);
                intent.putExtra(Intent.EXTRA_WIPE_ESIMS, false);
                sendBroadcast(intent);
            }
        };
    };


    public static final String RESET_RESULT = "reset_result";

    private void stopTest(boolean isOk) {
        // TODO Auto-generated method stub
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        byte result = -1;
        Editor resetEditor = mSharedPreferences.edit();
        if (isOk) {
//			恢复出厂设置成功时设置为-2
            result = -2;
            resetEditor.putInt(TestUtils.RESET_KEY + TestUtils.TEST_RESULT, 1);
            resetEditor.commit();
        } else {
//			恢复出厂设置失败时设置为-3
            result = -3;
            resetEditor.putInt(TestUtils.RESET_KEY + TestUtils.TEST_RESULT, 0);
            resetEditor.commit();
        }
        try {
            byte[] buff = new byte[1];
            buff[0] = result;
            NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_RESET_CURRENT_TIME, buff);
        } catch (RemoteException e) {
            // TODO: handle exception
        }
        //测试成功存-2，失败存-3
        //1,finish 2,startReport
        mHandler.removeMessages(RESET);

        Intent intent = new Intent(this, ReportActivity.class);
        intent.putExtra(RESET_RESULT, isOk ? 1 : 0);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
        startActivity(intent);
        finish();
    }

}
