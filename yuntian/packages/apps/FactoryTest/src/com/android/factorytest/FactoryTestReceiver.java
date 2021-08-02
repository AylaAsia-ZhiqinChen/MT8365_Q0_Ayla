package com.android.factorytest;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
//yuntian longyao add
//Description:暗码控制隐藏和显示水印
import android.widget.Toast;
import com.mediatek.nvram.NvRAMUtils;
//yuntian longyao end
/**
 * 工厂测试应用广播接收器，主要用于接收启动暗码
 */
public class FactoryTestReceiver extends BroadcastReceiver {
    //yuntian longyao add
    //Description:暗码控制隐藏和显示水印
    public static final String SECRET_CODE_ACTION = "android.provider.Telephony.SECRET_CODE";
    public Context mContext;
    //yuntian longyao end
    @Override
    public void onReceive(Context context, Intent intent) {
        mContext = context;
        String action = intent.getAction();
        Log.i(this, "onReceive=>action: " + action);
        if (Intent.ACTION_BOOT_COMPLETED.equals(action)) {
            if (context.getResources().getBoolean(R.bool.enabled_fighting_test)) {
                TestWatermarkService.launcherReset(context);
            }
        }
        //yuntian longyao add
        //Description:暗码控制隐藏和显示水印
        else if (SECRET_CODE_ACTION.equals(intent.getAction())) {
            Uri uri = intent.getData();
            Log.i(this, "onReceive=>uri: " + uri);
            Uri showhideKey = Uri.parse("android_secret_code://" + context.getString(R.string.show_hide_watermark_code));
            if (showhideKey.equals(uri)) {
                if (readWaterMarkFlagToNV()) {
                    writeWaterMarkFlagToNV(false);
                    Toast.makeText(context, R.string.show_watermark, Toast.LENGTH_LONG).show();
                } else {
                    writeWaterMarkFlagToNV(true);
                    Toast.makeText(context, R.string.hide_watermark, Toast.LENGTH_LONG).show();
                }
            }
        }
        //yuntian longyao end
    }

    //yuntian longyao add
    //Description:暗码控制隐藏和显示水印
    private void writeWaterMarkFlagToNV(boolean pass) {
        byte[] buff = null;
        try {
            buff = new byte[1];
            buff[0] = (byte) (pass ? 'P' : 'F');
            NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_SHOW_HIDE_WATERMARK_FLAG, buff);
            TestWatermarkService.launcherCheck(mContext);
        } catch (Exception e) {
            Log.d(this, "writeFmFlagToNV=>error: ", e);
        }
    }

    private boolean readWaterMarkFlagToNV() {
        try {
            byte[] secretCode = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_SHOW_HIDE_WATERMARK_FLAG, NvRAMUtils.SHOW_HIDE_WATERMARK_FLAG_LENGTH);
            Log.d(this, "secretCode=>value: " + new String(secretCode));
            if (secretCode[0] == 'P') {
                return true;
            }
        } catch (Exception e) {
            Log.e(this, "SMT get show hide watermark flag=>error: ", e);
        }
        return false;
    }
    //yuntian longyao end
}
