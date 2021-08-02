package com.android.imeisettings;

import android.content.Context;
import android.content.res.Resources;
import android.os.Handler;
import android.os.SystemProperties;
import com.mediatek.nvram.NvRAMUtils;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.widget.Toast;
import android.content.Intent;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

import java.util.Arrays;
import java.util.regex.Pattern;
//yuntian longyao add
//Description:非机主模式下禁止设置imei号
import android.os.UserHandle;
//yuntian longyao end

public class SettingHelper {

    public static final int MSG_SETTING_SIM1_IMEI = 0;
    public static final int MSG_SETTING_SIM2_IMEI = 1;
    public static final int MSG_RESTART_GSM_MODE = 3;
    public static final int MSG_SETTING_SIM1_IMEI_RESULT = 4;
    public static final int MSG_SETTING_SIM2_IMEI_RESULT = 5;
    public static final int MSG_RESTART_GSM_MODE_RESULT = 6;
    public static final int MSG_NOTIFY_IMEI_CHANGED = 7;
    //Yuntian:do not display notification 2017-08-03 21:56 by yuntian05 {{&&
    //Description:概率不弹出写号完成，请重启手机的提示
    //public static final int NOTIFY_IMEI_CHANGED_DELAYED = 10000;
    public static final int NOTIFY_IMEI_CHANGED_DELAYED = 15000;
   //&&}}
    private Context mContext;
    private Handler mHandler;

    private int mSim1ImeiLength;
    private int mSim2ImeiLength;
    private boolean mEnabledSettingSim1Imei;
    private boolean mEnabledSettingSim2Imei;
    private boolean mEnabledRespectivelySet;

    public SettingHelper(Context context, Handler handler) {
        mContext = context;
        mHandler = handler;

        Resources res = mContext.getResources();
        mSim1ImeiLength = res.getInteger(R.integer.sim1_imei_length);
        mSim2ImeiLength = res.getInteger(R.integer.sim2_imei_length);
        mEnabledSettingSim1Imei = res.getBoolean(R.bool.enabled_setting_sim1_imei);
        mEnabledSettingSim2Imei = res.getBoolean(R.bool.enabled_setting_sim2_imei);
        mEnabledRespectivelySet = res.getBoolean(R.bool.enabled_respectively_set);

        Log.d(this, "SettingHelper=>sim1: " + mEnabledSettingSim1Imei + " sim2: " + mEnabledSettingSim2Imei
                + " resPectivelySet: " + mEnabledRespectivelySet);
    }

    public boolean enabledSettingImei(String sim1Imei, String sim2Imei, boolean showTip) {
        Log.d(this, "enabledSettingImei=>imei1: " + sim1Imei.trim() + " imei2: " + sim2Imei.trim());
        boolean result = true;

        if (!checkSim1Imei(sim1Imei.trim(), showTip)) {
            result = false;
        } else if (!checkSim2Imei(sim2Imei.trim(), showTip)) {
            result = false;
        }

        Log.d(this, "enabledSettingImei=>result: " + result);
        return result;
    }

    public void restartGsmMode() {
        //yuntian longyao add
        //Description:非机主模式下禁止设置imei号
        if (UserHandle.myUserId() == UserHandle.USER_OWNER) {
            final String[] command = new String[2];
            command[0] = "AT+EPON";
            command[1] = "+EPON";
            Phone phone = PhoneFactory.getDefaultPhone();
            Log.d(this, "restartGsmMode=>radioOn: " + phone.isRadioOn() + " available: " + phone.isRadioAvailable());
            phone.invokeOemRilRequestStrings(command, mHandler != null ? mHandler.obtainMessage(MSG_RESTART_GSM_MODE_RESULT) : null);
        } else {
            Toast.makeText(mContext, mContext.getString(R.string.forbid_setting_imei), Toast.LENGTH_SHORT).show();
        }
        //yuntian longyao end
    }

    public void executeWriteSim1ImeiATCommand(String imei) {
        //yuntian longyao add
        //Description:非机主模式下禁止设置imei号
        if (UserHandle.myUserId() == UserHandle.USER_OWNER) {
            Log.d(this, "settingSim1Imei=>imei: " + imei);
            Phone phone = PhoneFactory.getDefaultPhone();
            Log.d(this, "executeWriteSim1ImeiATCommand=>radioOn: " + phone.isRadioOn() + " available: " + phone.isRadioAvailable());
            String[] commands = new String[2];
            commands[0] = "AT+EGMR=1,7,\"" + imei + "\"";
            commands[1] = "+EGMR";
            phone.invokeOemRilRequestStrings(commands, mHandler != null ? mHandler.obtainMessage(MSG_SETTING_SIM1_IMEI_RESULT) : null);
        } else {
            Toast.makeText(mContext, mContext.getString(R.string.forbid_setting_imei), Toast.LENGTH_SHORT).show();
        }
        //yuntian longyao end
    }

    public void executeWriteSim2ImeiATCommand(String imei) {
        //yuntian longyao add
        //Description:非机主模式下禁止设置imei号
        if (UserHandle.myUserId() == UserHandle.USER_OWNER) {
            Log.d(this, "settingSim2Imei=>imei: " + imei);
            Phone phone = PhoneFactory.getDefaultPhone();
            Log.d(this, "executeWriteSim2ImeiATCommand=>radioOn: " + phone.isRadioOn() + " available: " + phone.isRadioAvailable());
            String[] commands = new String[2];
            commands[0] = "AT+EGMR=1,10,\"" + imei + "\"";
            commands[1] = "+EGMR";
            phone.invokeOemRilRequestStrings(commands, mHandler != null ? mHandler.obtainMessage(MSG_SETTING_SIM2_IMEI_RESULT) : null);
        } else {
            Toast.makeText(mContext, mContext.getString(R.string.forbid_setting_imei), Toast.LENGTH_SHORT).show();
        }
        //yuntian longyao end
    }

    public boolean writeSim1ImeiToNv(String imei) {
        Log.d(this, "writeSim1ImeiToNv=>imei: " + imei);
        byte[] bytes = stringToBytes(imei, mSim1ImeiLength);
        bytes = Arrays.copyOf(bytes, 10);
        try {
            return NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_SIM1_IMEI, bytes);
        } catch (Exception e) {
            Log.e(this, "writeSim1ImeiToNv=>error: ", e);
            return false;
        }
    }

    public boolean writeSim2ImeiToNv(String imei) {
        Log.d(this, "writeSim2ImeiToNv=>imei: " + imei);
        byte[] bytes = stringToBytes(imei, mSim2ImeiLength);
        bytes = Arrays.copyOf(bytes, 10);
        try {
            return NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_SIM2_IMEI, bytes);
        } catch (Exception e) {
            Log.e(this, "writeSim2ImeiToNv=>error: ", e);
            return false;
        }
    }

    public String getSim1ImeiFromNv() {
        String imei = null;
        try {
            byte[] bytes = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_SIM1_IMEI, NvRAMUtils.SIM1_IMEI_LENGTH);
            imei = bytesToString(bytes, mSim1ImeiLength);
            Log.d(this, "getSim1ImeiFromNv=>imei: " + imei);
            if (!isLegitimateImei(imei, mSim1ImeiLength)) {
                imei = "";
            }
        } catch (Exception e) {
            Log.e(this, "getSim1ImeiFromNv=>error: ", e);
        }
        return imei;
    }

    public String getSim2ImeiFromNv() {
        String imei = null;
        try {
            byte[] bytes = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_SIM2_IMEI, NvRAMUtils.SIM2_IMEI_LENGTH);
            imei = bytesToString(bytes, mSim2ImeiLength);
            Log.d(this, "getSim2ImeiFromNv=>imei: " + imei);
            if (!isLegitimateImei(imei, mSim2ImeiLength)) {
                imei = "";
            }
        } catch (Exception e) {
            Log.e(this, "getSim2ImeiFromNv=>error: ", e);
        }
        return imei.trim();
    }

    public int getSettingCount(String sim1Imei, String sim2Imei) {
        Log.d(this, "getSettingCount=>imei1: " + sim1Imei + " imei2: " + sim2Imei);
        int count = 0;

        if (needSettingSim1Imei(sim1Imei)) {
            count++;
        }
        if (needSettingSim2Imei(sim2Imei)) {
            count++;
        }

        Log.d(this, "getSettingCount=>count: " + count);
        return count;
    }

    public boolean checkSim1Imei(String imei, boolean showTip) {
        boolean result = true;
        if (mEnabledSettingSim1Imei) {
            if (mEnabledRespectivelySet) {
                if (!TextUtils.isEmpty(imei) && imei.length() != mSim1ImeiLength) {
                    result = false;
                    if (showTip) {
                        Toast.makeText(mContext, mContext.getString(R.string.sim1_imei_length_limit_tip, mSim1ImeiLength), Toast.LENGTH_SHORT).show();
                    }
                } else if ("000000000000000".equals(imei)) {
                    result = false;
                    if (showTip) {
                        Toast.makeText(mContext, mContext.getString(R.string.sim1_imei_value_limit_tip, "000000000000000"), Toast.LENGTH_SHORT).show();
                    }
                }
            } else {
                if (imei.length() != mSim1ImeiLength) {
                    result = false;
                    if (showTip) {
                        Toast.makeText(mContext, mContext.getString(R.string.sim1_imei_length_limit_tip, mSim1ImeiLength), Toast.LENGTH_SHORT).show();
                    }
                } else if ("000000000000000".equals(imei)) {
                    result = false;
                    if (showTip) {
                        Toast.makeText(mContext, mContext.getString(R.string.sim1_imei_value_limit_tip, "000000000000000"), Toast.LENGTH_SHORT).show();
                    }
                }
            }
        }
        return result;
    }

    public boolean checkSim2Imei(String imei, boolean showTip) {
        boolean result = true;
        if (mEnabledSettingSim2Imei) {
            if (mEnabledRespectivelySet) {
                if (!TextUtils.isEmpty(imei) && imei.length() != mSim2ImeiLength) {
                    result = false;
                    if (showTip) {
                        Toast.makeText(mContext, mContext.getString(R.string.sim2_imei_length_limit_tip, mSim2ImeiLength), Toast.LENGTH_SHORT).show();
                    }
                } else if ("000000000000000".equals(imei)) {
                    result = false;
                    if (showTip) {
                        Toast.makeText(mContext, mContext.getString(R.string.sim2_imei_value_limit_tip, "000000000000000"), Toast.LENGTH_SHORT).show();
                    }
                }
            } else {
                if (imei.length() != mSim2ImeiLength) {
                    result = false;
                    if (showTip) {
                        Toast.makeText(mContext, mContext.getString(R.string.sim2_imei_length_limit_tip, mSim2ImeiLength), Toast.LENGTH_SHORT).show();
                    }
                } else if ("000000000000000".equals(imei)) {
                    result = false;
                    if (showTip) {
                        Toast.makeText(mContext, mContext.getString(R.string.sim2_imei_value_limit_tip, "000000000000000"), Toast.LENGTH_SHORT).show();
                    }
                }
            }
        }
        return result;
    }

    public boolean needSettingSim1Imei(String imei) {
        boolean need = false;
        if (mEnabledSettingSim1Imei) {
            if (imei.length() == mSim1ImeiLength) {
                need = true;
            }
        }
        return need;
    }

    public boolean needSettingSim2Imei(String imei) {
        boolean need = false;
        if (mEnabledSettingSim2Imei) {
            if (imei.length() == mSim2ImeiLength) {
                need = true;
            }
        }
        return need;
    }

    public byte[] stringToBytes(String str, int length) {
        if (str == null || "".equals(str)) {
            return null;
        }
        int len = length / 2;
        if (length % 2 != 0) {
            len++;
        }
        byte[] bytes = new byte[len];
        for (int i = 0, j = 0; i < str.length() && j < len; i += 2, j++) {
            if (i + 1 >= str.length()) {
                bytes[j] = (byte) str.charAt(i);
            } else {
                bytes[j] = mergeByte(str.charAt(i), str.charAt(i + 1));
            }
        }
        return bytes;
    }

    public byte mergeByte(char first, char second) {
        int f = Integer.parseInt(String.valueOf(first), 16);
        int s = Integer.parseInt(String.valueOf(second), 16);
        int result = f << 4;
        result |= s;
        return (byte) (result & 0xFF);
    }

    public String bytesToString(byte[] bytes, int length) {
        String result = "";
        if (bytes != null && bytes.length != 0) {
            byte[] realBytes = new byte[length];
            for (int i = 0, j = 0; i < bytes.length && j < length; i++, j += 2) {
                if (j + 1 >= length) {
                    realBytes[j] = bytes[i];
                } else {
                    realBytes[j] = getHightByte(bytes[i]);
                    realBytes[j + 1] = getLowByte(bytes[i]);
                }
            }
            if (!isUninitializedValue(realBytes)) {
                result = new String(realBytes).trim().toUpperCase();
            }
        }
        return result;
    }

    public boolean isUninitializedValue(byte[] bytes) {
        boolean result = true;
        for (int i = 0; i < bytes.length; i++) {
            if (bytes[i] != 0) {
                result = false;
            }
        }
        return result;
    }

    public byte getHightByte(byte b) {
        return Integer.toString(((b & 0xFF) >>> 4), 16).getBytes()[0];
    }

    public byte getLowByte(byte b) {
        return Integer.toString((b & 0xF), 16).getBytes()[0];
    }

    public boolean isLegitimateImei(String imei, int length) {
        String regex = "[0-9]*";
        return (imei != null && imei.length() == length && Pattern.compile(regex).matcher(imei).matches());
    }

    public boolean needSwapImei() {
        String swap = SystemProperties.get("persist.radio.simswitch", "0");
        return false;//swap.equals("2");
    }

    public String getDeviceId(int slotId) {
        TelephonyManager tm = (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
        if (slotId >= 0 && slotId < tm.getPhoneCount()) {
            
            //Yuntian:auto boot control for cts test at 2017-08-29 14:13 by yuntian22 {{&&
            //return PhoneFactory.getPhone(slotId).getImei();
			try{
                  return PhoneFactory.getPhone(slotId).getImei();
		    }catch(IllegalStateException e){
				  return null;
			}
		    //&&}}
        }
        return null;
    }

    public boolean clearNV() {
        boolean result = false;
        try {
            result = NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_SIM1_IMEI, new byte[NvRAMUtils.INDEX_SIM2_IMEI + NvRAMUtils.SIM2_IMEI_LENGTH - NvRAMUtils.INDEX_SIM1_IMEI]);
        } catch (Exception e) {
            Log.e(this, "clearNV=>error: ", e);
        }
        return result;
    }
    
    public void sendImeiChangedBroadcast() {
		Intent imeiChanged = new Intent("com.android.imeisettings.IMEI_CHANGED");
		imeiChanged.setPackage("com.android.watermark");
		mContext.sendBroadcast(imeiChanged);
	}
}
