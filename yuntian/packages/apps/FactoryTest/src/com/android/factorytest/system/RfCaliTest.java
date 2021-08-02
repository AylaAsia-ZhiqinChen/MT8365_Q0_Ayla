package com.android.factorytest.system;

import android.os.Bundle;
import android.os.SystemProperties;
import android.text.Spannable;
import android.text.SpannableStringBuilder;
import android.text.TextUtils;
import android.text.style.ForegroundColorSpan;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;
import android.util.Log;
import android.view.View;
import com.mediatek.nvram.NvRAMUtils;
import android.content.res.Resources;

/**
 * 校准综测测试
 */
public class RfCaliTest extends BaseActivity {

    private TextView mGsmSerialTv;
    private TextView mSogouSN1Tv;
    private TextView mSogouSNNTv;
    private TextView mCalibrationTv;
    private TextView mComprehensiveSurveyTv;
    private String TAG = "RfCaliTest";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_rf_cali_test);

        super.onCreate(savedInstanceState);

        mGsmSerialTv = (TextView) findViewById(R.id.gsm_serial);
        mSogouSN1Tv = (TextView) findViewById(R.id.sogou_sn1);
        mSogouSNNTv = (TextView) findViewById(R.id.sogou_snn);
        mCalibrationTv = (TextView) findViewById(R.id.calibration);
        mComprehensiveSurveyTv = (TextView) findViewById(R.id.comprehensive_survey);
    }

    @Override
    protected void onResume() {
        super.onResume();
        String serial = getGSMSerial();//android.os.Build.getSerial()//如果要显示sn号，就用这个getSerial
        String sn1 = readSN1ToNV();
        String snn = readSNNToNV();
        boolean isCalibrationPass = isCalibrationPass();
        boolean isComprehensiveSurverPass = isComprehensiveSurveyPass();

        int normalColor = getColor(R.color.text_view_text_color);
        int redColor = getColor(R.color.red);
        int greenColor = getColor(R.color.green);
        boolean enabledPass = true;
        Resources res = getResources();
        String info = null;
        String title = getString(R.string.gsm_serial_test_title);
        SpannableStringBuilder builder = null;
        ForegroundColorSpan blueSpan = null;
        if (!TextUtils.isEmpty(serial)) {
            info = title + serial;
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(greenColor);
        } else {
            enabledPass = false;
            info = title + "null";
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(redColor);
        }
        builder.setSpan(blueSpan, title.length(), info.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        mGsmSerialTv.setText(builder);

        title = getString(R.string.sn1_test_title);
        if (!TextUtils.isEmpty(sn1)) {
            info = title + sn1;
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(greenColor);
        } else {
            if (res.getBoolean(R.bool.support_sogou_sn1)) {
                enabledPass = false;
            }
            info = title + "null";
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(redColor);
        }
        builder.setSpan(blueSpan, title.length(), info.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        mSogouSN1Tv.setText(builder);
        if (!res.getBoolean(R.bool.support_sogou_sn1)) {
            mSogouSN1Tv.setVisibility(View.GONE);
        }

        title = getString(R.string.snn_test_title);
        if (!TextUtils.isEmpty(snn)) {
            info = title + snn;
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(greenColor);
        } else {
            if (res.getBoolean(R.bool.support_sogou_snn)) {
                enabledPass = false;
            }
            info = title + "null";
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(redColor);
        }
        builder.setSpan(blueSpan, title.length(), info.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        mSogouSNNTv.setText(builder);
        if (!res.getBoolean(R.bool.support_sogou_snn)) {
            mSogouSNNTv.setVisibility(View.GONE);
        }

        title = getString(R.string.calibration_test_title);
        if (isCalibrationPass) {
            info = title + getString(R.string.pass);
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(greenColor);
        } else {
            enabledPass = false;
            info = title + getString(R.string.fail);
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(redColor);
        }
        builder.setSpan(blueSpan, title.length(), info.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        mCalibrationTv.setText(builder);

        title = getString(R.string.comprehensive_surver_test_title);
        if (isComprehensiveSurverPass) {
            info = title + getString(R.string.pass);
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(greenColor);
        } else {
            enabledPass = false;
            info = title + getString(R.string.fail);
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(redColor);
        }
        builder.setSpan(blueSpan, title.length(), info.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        mComprehensiveSurveyTv.setText(builder);

        setTestCompleted(true);
        if (enabledPass) {
            setPassButtonEnabled(true);
            setTestPass(true);
        } else {
            setPassButtonEnabled(false);
            setTestPass(false);
        }
        if (isAutoTest()) {
            if (isTestPass()) {
                Toast.makeText(this, getString(R.string.auto_test_pass_tip, getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(this, getString(R.string.auto_test_fail_tip, getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
            }
            doOnAutoTest();
        }
    }

    /**
     * 获取GSM序列号
     * @return 返回GSM序列号
     */
    public String getGSMSerial() {
        return SystemProperties.get("vendor.gsm.serial");
    }

    private void writeSN1ToNV() {
        String str = "sugou_sn1_22222222222222";
        byte[] srtbyte = str.getBytes();
        try {
            NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_SOGOU_SN_FLAG, srtbyte);
        } catch (Exception e) {
            Log.d(TAG, "writeSN1ToNV=>error: ", e);
        }
    }

    private String readSN1ToNV() {
        try {
            byte[] sn1 = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_SOGOU_SN_FLAG, NvRAMUtils.SOGOU_SN_FLAG_LENGTH);
            String sogouSN1 = new String(sn1).trim();
            Log.i(TAG, "sogouSN1=>value: " + sogouSN1);
            if (!TextUtils.isEmpty(sogouSN1)) {
                return sogouSN1;
            }
        } catch (Exception e) {
            Log.e(TAG, "readSN1ToNV=>error: ", e);
        }
        return null;
    }

    private void writeSNNToNV() {
        String str = "sugou_snn_66666666666666";
        byte[] srtbyte = str.getBytes();
        try {
            NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_BOARD_SN_FLAG, srtbyte);
        } catch (Exception e) {
            Log.d(TAG, "writeSNNToNV=>error: ", e);
        }
    }

    private String readSNNToNV() {
        try {
            byte[] snn = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_BOARD_SN_FLAG, NvRAMUtils.SOGOU_BOARD_LENGTH);
            String sogouSNN = new String(snn).trim();
            Log.i(TAG, "sogouSNN=>value: " + sogouSNN);
            if (!TextUtils.isEmpty(sogouSNN)) {
                return sogouSNN;
            }
        } catch (Exception e) {
            Log.e(TAG, "readSNNToNV=>error: ", e);
        }
        return null;
    }

    /**
     * 判断设备是否校准过
     * 通过判断GSM序列号第60位和61位（从０位算起）的值是否是10来判断该设备是否校准过
     * @return 如果设备校准过，返回true；否则返回false
     */
    public boolean isCalibrationPass() {
        boolean isCalibration = false;
        String gsmSerial = getGSMSerial();
        if (!TextUtils.isEmpty(gsmSerial) && gsmSerial.length() >= 62) {
            if (gsmSerial.charAt(60) == '1' && gsmSerial.charAt(61) == '0') {
                isCalibration = true;
            }
        }
        return isCalibration;
    }

    /**
     * 判断设备是否综测过
     * 通过判断GSM序列号第6２位（从０位算起）的值是否是P来判断该设备是否综测过
     * @return 如果设备综测过，返回true；否则返回false
     */
    public boolean isComprehensiveSurveyPass() {
        boolean isComprehensiveSurvey = false;
        String gsmSerial = getGSMSerial();
        if (!TextUtils.isEmpty(gsmSerial) && gsmSerial.length() >= 63) {
            if (gsmSerial.charAt(62) == 'P') {
                isComprehensiveSurvey = true;
            }
        }
        return isComprehensiveSurvey;
    }
}
