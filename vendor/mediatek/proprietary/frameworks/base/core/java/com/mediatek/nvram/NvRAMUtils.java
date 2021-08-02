package com.mediatek.nvram;

import android.os.RemoteException;
import android.os.ServiceManager;

import vendor.mediatek.hardware.nvram.V1_0.INvram;

import java.util.ArrayList;

import android.util.Log;

import com.android.internal.util.HexDump;

public class NvRAMUtils {

    private static final String TAG = "NvRAMUtils";

    private static final String PRODUCT_INFO_FILENAME = "/mnt/vendor/nvdata/APCFG/APRDEB/PRODUCT_INFO";
    private static final String YUNTIAN_PRODUCT_INFO_FILENAME = "/mnt/vendor/nvdata/APCFG/APRDEB/YUNTIAN_PRO_INFO";

    /***** NvRAMUtils - Place your offset below *****/
    //@Description: Ensure there is no duplicated offset declared!
    // NV offset - Please Try to use the index between 1024 - 2047
    public static final int NV_OFFSET = 104;
    public static final int NV_LENGTH = 1024;

    /// Yuntian: ImeiSettings at 2017-02-16 10:50:02 by yuntian6{{&&	
    /// Description: 添加IMEI1和IMEI2 NV下标
    public static final int INDEX_SIM1_IMEI = 64;
    public static final int INDEX_SIM2_IMEI = 74;
    public static final int SIM1_IMEI_LENGTH = 10;
    public static final int SIM2_IMEI_LENGTH = 10;
    ///&&}}
//yuntian andy add for factory test begin
    public static final int INDEX_FACTORY_TEST_FLAG = 59;
    public static final int FACTORY_TEST_FLAG_LENGTH = 1;
    //yuntian andy add for factory test end
    ///Author: 2017-02-21 by yuntian1{{&&
    ///Description:Agingtest中为恢复出厂设置测试使用
    ///记录需要恢复出厂测试的总次数
    public static final int INDEX_RESET_ALL_TIMES = NV_OFFSET + 0;
    ///记录当前恢复出厂的次数
    public static final int INDEX_RESET_CURRENT_TIME = NV_OFFSET + 1;
    public static final int RESET_ALL_TIMES_LENGTH = 1;
    public static final int RESET_CURRENT_TIME_LENGTH = 1;
    /// &&}}
    ///Author: 2018-02-28 by yuntian05{{&&
    ///Description:MengSalertracker中销量统计使用
    public static final int INDEX_TRACKER_SMS_FLAG = NV_OFFSET + 2;
    public static final int TRACKER_SMS_FLAG_LENGTH = 1;
    /// &&}}	
//yuntian andy add for factory test begin
    public static final int INDEX_MMI_TEST_FLAG = NV_OFFSET + 3;
    public static final int MMI_TEST_FLAG_LENGTH = 1;

    public static final int INDEX_FACTORY_TEST_NEW_FLAG = NV_OFFSET + 4;
    public static final int FACTORY_TEST_FLAG_NEW_LENGTH = 1;
//yuntian andy add for factory test end

    //yuntian longyao add
    //Description:暗码控制隐藏和显示水印
    public static final int INDEX_SHOW_HIDE_WATERMARK_FLAG = NV_OFFSET + 5;
    public static final int SHOW_HIDE_WATERMARK_FLAG_LENGTH = 1;
    //yuntian longyao end

    //yuntian chunlei add
    //Description:pdt 遥毙状态
    public static final int INDEX_PDT_HARUKA_FLAG = NV_OFFSET + 6;
    public static final int PDT_HARUKA_FLAG_LENGTH = 1;
    //yuntian chunlei end

    //yuntian  add
    //Description: 自定义SSN号
    public static final int INDEX_SOGOU_SN_FLAG = NV_OFFSET + 7;
    public static final int SOGOU_SN_FLAG_LENGTH = 64;
    public static final int INDEX_BOARD_SN_FLAG = NV_OFFSET + 71;
    public static final int SOGOU_BOARD_LENGTH = 64;
    //yuntian  end

    public static final int INDEX_AGINGTEST_REBOOT = NV_OFFSET + 135;
    public static final int INDEX_AGINGTEST_REBOOT_LENGTH = 1;

    public static final int INDEX_AGINGTEST_SLEEP = NV_OFFSET + 136;
    public static final int INDEX_AGINGTEST_SLEEP_LENGTH = 1;

    public static final int INDEX_AGINGTEST_VIBRATION = NV_OFFSET + 137;
    public static final int INDEX_AGINGTEST_VIBRATION_LENGTH = 1;


    public static final int INDEX_AGINGTEST_FRONT_CAMERA = NV_OFFSET + 138;
    public static final int INDEX_AGINGTEST_FRONT_CAMERA_LENGTH = 1;


    public static final int INDEX_AGINGTEST_BACK_CAMERA = NV_OFFSET + 139;
    public static final int INDEX_AGINGTEST_BACK_CAMERA_LENGTH = 1;

    public static final int INDEX_AGINGTEST_VIDEO = NV_OFFSET + 140;
    public static final int INDEX_AGINGTEST_VIDEO_LENGTH = 1;

    public static final int INDEX_AGINGTEST_MOTOR = NV_OFFSET + 141;
    public static final int INDEX_AGINGTEST_MOTOR_LENGTH = 1;

    public static final int INDEX_AGINGTEST_FLASH = NV_OFFSET + 142;
    public static final int INDEX_AGINGTEST_FLASH_LENGTH = 1;

    public static final int INDEX_AGINGTEST_SPEAKER = NV_OFFSET + 143;
    public static final int INDEX_AGINGTEST_SPEAKER_LENGTH = 1;

    public static final int INDEX_AGINGTEST_SCREEN = NV_OFFSET + 144;
    public static final int INDEX_AGINGTEST_SCREEN_LENGTH = 1;

    public static final int INDEX_AGINGTEST_RECEIVER = NV_OFFSET + 145;
    public static final int INDEX_AGINGTEST_RECEIVER_LENGTH = 1;

    public static final int INDEX_AYLA_DSN = NV_OFFSET + 146;
    public static final int INDEX_AYLA_DSN_LENGTH = 50;

    public static final int INDEX_AYLA_RSA_PUB_KEY = NV_OFFSET + 196;
    public static final int INDEX_AYLA_RSA_PUB_KEY_LENGTH = 500;

    // Declared offsets above should be included in this integer array, otherwise verification and I/O afterwards will fail
    private static final int[] INDEX_LIST = {
            //yuntian andy add for factory test begin
            INDEX_FACTORY_TEST_FLAG,
            //yuntian andy add for factory test end
            /// Yuntian: ImeiSettings at 2017-02-16 10:50:02 by yuntian6{{&&
            /// Description: 添加IMEI1和IMEI2 NV下标
            INDEX_SIM1_IMEI,
            INDEX_SIM2_IMEI,
            ///&&}}
            ///Author: 2017-02-21 by yuntian1{{&&
            ///Description:Agingtest中为恢复出厂设置测试使用
            INDEX_RESET_ALL_TIMES,
            INDEX_RESET_CURRENT_TIME,
            ///&&}}
            ///Description:MengSalertracker中为销量统计使用
            INDEX_TRACKER_SMS_FLAG,
            //yuntian andy add for factory test begin
            INDEX_MMI_TEST_FLAG,
            INDEX_FACTORY_TEST_NEW_FLAG,

            //yuntian andy add for factory test end
            //yuntian longyao add
            //Description:暗码控制隐藏和显示水印
            INDEX_SHOW_HIDE_WATERMARK_FLAG,
            //yuntian longyao end            
            /// &&}}

            //yuntian chunlei add
            //Description:PDT 遥毙状态
            INDEX_PDT_HARUKA_FLAG,
            //yuntian chunlei end           

            //yuntian  add
            //Description: 自定义SSN号
            INDEX_SOGOU_SN_FLAG,
            //yuntian  end
            //yuntian  add
            //Description: 自定义工厂SN号
            INDEX_BOARD_SN_FLAG,
            //yuntian  end
            INDEX_AGINGTEST_REBOOT,
            INDEX_AGINGTEST_SLEEP,
            INDEX_AGINGTEST_VIBRATION,
            INDEX_AGINGTEST_FRONT_CAMERA,
            INDEX_AGINGTEST_BACK_CAMERA,
            INDEX_AGINGTEST_VIDEO,
            INDEX_AGINGTEST_MOTOR,
            INDEX_AGINGTEST_FLASH,
            INDEX_AGINGTEST_SPEAKER,
            INDEX_AGINGTEST_SCREEN,
            INDEX_AGINGTEST_RECEIVER,
            INDEX_AYLA_DSN,
            INDEX_AYLA_RSA_PUB_KEY
    };


    public synchronized static byte[] readNV(int index, int length) throws RemoteException {
        StringBuffer nvramBuf = new StringBuffer();
        try {
            int i = 0;
            String buff = null;
            INvram agent = INvram.getService();
            if (agent == null) {
                Log.e(TAG, "NvRAMAgent is null");
                return null;
            }
            try {
                buff = agent.readFileByName(PRODUCT_INFO_FILENAME, index + length);
            } catch (Exception e) {
                e.printStackTrace();
                return null;
            }
            Log.i(TAG, "Raw data:" + buff);
            if (buff.length() < 2 * (index + length)) {
                return null;
            }
            // Remove the \0 special character.
            int macLen = buff.length() - 1;
            for (i = index * 2; i < macLen; i += 2) {
                if ((i + 2) < macLen) {
                    nvramBuf.append(buff.substring(i, i + 2));
                } else {
                    nvramBuf.append(buff.substring(i));
                }
            }
            Log.d(TAG, "buff:" + nvramBuf.toString());
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
        String str = nvramBuf.toString();
        byte[] bytes = HexDump.hexStringToByteArray(
                str.substring(0, str.length() - 1));

        return bytes;
    }

    public synchronized static byte[] readNVFromYt(int index, int length) throws RemoteException {
        StringBuffer nvramBuf = new StringBuffer();
        try {
            int i = 0;
            String buff = null;
            INvram agent = INvram.getService();
            if (agent == null) {
                Log.e(TAG, "NvRAMAgent is null");
                return null;
            }
            try {
                buff = agent.readFileByName(YUNTIAN_PRODUCT_INFO_FILENAME, index + length);
            } catch (Exception e) {
                e.printStackTrace();
                return null;
            }
            Log.i(TAG, "Raw data:" + buff);
            if (buff.length() < 2 * (index + length)) {
                return null;
            }
            // Remove the \0 special character.
            int macLen = buff.length() - 1;
            for (i = index * 2; i < macLen; i += 2) {
                if ((i + 2) < macLen) {
                    nvramBuf.append(buff.substring(i, i + 2));
                } else {
                    nvramBuf.append(buff.substring(i));
                }
            }
            Log.d(TAG, "buff:" + nvramBuf.toString());
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
        String str = nvramBuf.toString();
        byte[] bytes = HexDump.hexStringToByteArray(
                str.substring(0, str.length() - 1));

        return bytes;
    }

    /**
     * write a specified byte array into NvRAM, start with the specified index
     *
     * @param index the start position that need to be written
     * @param buff  the values that need to be written from the start position
     * @return true when write succeeded, false when write failed
     */
    public synchronized static boolean writeNV(int index, byte[] buff) throws RemoteException {
        try {
            int i = 0;
            INvram agent = INvram.getService();
            if (agent == null) {
                Log.e(TAG, "NvRAMAgent is null");
                return false;
            }
            byte[] buffArr = new byte[NV_LENGTH];
            try {
                buffArr = readNV(0, NV_LENGTH);
            } catch (Exception e) {
                e.printStackTrace();
                return false;
            }

            for (i = 0; i < buff.length; i++) {
                buffArr[i + index] = buff[i];
            }
            ArrayList<Byte> dataArray = new ArrayList<Byte>(
                    NV_LENGTH);
            for (i = 0; i < NV_LENGTH; i++) {
                dataArray.add(i, new Byte(buffArr[i]));
            }
            int flag = 0;
            try {
                flag = agent.writeFileByNamevec(PRODUCT_INFO_FILENAME,
                        NV_LENGTH, dataArray);
            } catch (Exception e) {
                e.printStackTrace();
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;

    }

    public synchronized static boolean writeNVToYt(int index, byte[] buff) throws RemoteException {
        try {
            int i = 0;
            INvram agent = INvram.getService();
            if (agent == null) {
                Log.e(TAG, "NvRAMAgent is null");
                return false;
            }
            byte[] buffArr = new byte[NV_LENGTH];
            try {
                buffArr = readNVFromYt(0, NV_LENGTH);
            } catch (Exception e) {
                e.printStackTrace();
                return false;
            }

            for (i = 0; i < buff.length; i++) {
                buffArr[i + index] = buff[i];
            }
            ArrayList<Byte> dataArray = new ArrayList<Byte>(
                    NV_LENGTH);
            for (i = 0; i < NV_LENGTH; i++) {
                dataArray.add(i, new Byte(buffArr[i]));
            }
            int flag = 0;
            try {
                flag = agent.writeFileByNamevec(YUNTIAN_PRODUCT_INFO_FILENAME,
                        NV_LENGTH, dataArray);
            } catch (Exception e) {
                e.printStackTrace();
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;

    }


    private synchronized static boolean verifyIndex(int index) {
        //ZDP - To check whether the index is declared in the Array INDEX_LIST
        for (int i = 0; i < INDEX_LIST.length; i++) {
            if (INDEX_LIST[i] == index) {
                return true;
            }
        }
        return false;
    }
    /***** NvRAMUtils - Generic I/O Methods end *****/
}
