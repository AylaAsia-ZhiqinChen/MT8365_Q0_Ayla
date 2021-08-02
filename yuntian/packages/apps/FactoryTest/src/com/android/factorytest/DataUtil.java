package com.android.factorytest;

import android.text.TextUtils;

/**
 * 16进制字符转换相关类
 */
public class DataUtil {
    private static String TAG = "DataUtil";

    public static final String KEY_SERIAL_PORTS  = "serial_ports";
    public static final String KEY_REFRESH_DELAYED_TIME = "refresh_delayed_time";
    public static final String KEY_CONVER_HEX_LOG_TO_STRING = "conver_hex_log_to_string";

    /**
     * 判断奇数或偶数，位运算，最后一位是1则为奇数，为0是偶数
     */
    public static int isOdd(int num) {
        return num & 0x1;
    }

    /**
     * Hex字符串转int
     * @param inHex
     * @return
     */
    public static int HexToInt(String inHex) {
        return Integer.parseInt(inHex, 16);
    }

    /**
     * Hex字符串转byte
     * @param inHex
     * @return
     */
    public static byte HexToByte(String inHex) {
        return (byte)Integer.parseInt(inHex,16);
    }

    /**
     * 1字节转2个Hex字符
     * @param inByte
     * @return
     */
    static public String Byte2Hex(Byte inByte) {
        return String.format("%02x", inByte).toUpperCase();
    }

    /**
     * 字节数组转转hex字符串
     * @param inBytArr
     * @return
     */
    public static String ByteArrToHex(byte[] inBytArr) {
        StringBuilder strBuilder = new StringBuilder();
        int j = inBytArr.length;
        for (int i = 0; i < j; i++) {
            strBuilder.append(Byte2Hex(inBytArr[i]));
            strBuilder.append(" ");
        }
        return strBuilder.toString();
    }

    /**
     * 字节数组转转hex字符串，可选长度
     * @param inBytArr
     * @param offset
     * @param byteCount
     * @return
     */
    static public String ByteArrToHex(byte[] inBytArr,int offset,int byteCount) {
        StringBuilder strBuilder = new StringBuilder();
        int j = byteCount;
        for (int i = offset; i < j; i++) {
            strBuilder.append(Byte2Hex(inBytArr[i]));
        }
        return strBuilder.toString();
    }
    
    /**
     * 字节数组转转hex小写字符串
     * @param src
     * @return
     */
    public static String bytesToHexString(byte[] src) {
        StringBuilder stringBuilder = new StringBuilder("");
        if (src == null || src.length <= 0) {
            return null;
        }
        for (int i = 0; i < src.length; i++) {
            int v = src[i] & 0xFF;
            String hv = Integer.toHexString(v);
            if (hv.length() < 2) {
                stringBuilder.append(0);
            }
            stringBuilder.append(hv);
        }
        return stringBuilder.toString();
    }

    /**
     * hex字符串转字节数组
     */
    static public byte[] HexToByteArr(String inHex) {
        int hexlen = inHex.length();
        byte[] result;
        if (isOdd(hexlen) == 1) {//奇数
            hexlen++;
            result = new byte[(hexlen/2)];
            inHex = "0"+inHex;
        } else {//偶数
            result = new byte[(hexlen/2)];
        }
        int j = 0;
        for (int i = 0; i < hexlen; i += 2) {
            result[j] = HexToByte(inHex.substring(i,i+2));
            j++;
        }
        return result;
    }

    public static boolean isHexString(String data) {
        String regex="^[A-Fa-f0-9]+$";
        // "[1]"代表第1位为数字1，"[3578]"代表第二位可以为3、5、8中的一个，"\\d{9}"代表后面是可以是0～9的数字，有9位。
        if (TextUtils.isEmpty(data)) {
            return false;
        } else {
            return data.matches(regex);
        }
    }
    
    /*产生numSize位16进制的数*/
    public static String getRandomString(int numSize) {
        String str = "";
        for (int i = 0; i < numSize; i++) {
            char temp = 0;
            int key = (int) (Math.random() * 2);
            switch (key) {
                case 0:
                    temp = (char) (Math.random() * 10 + 48);//产生随机数字
                    Log.e(TAG, "getRandomString........temp=" + temp);
                    break;
                case 1:
                    temp = (char) (Math.random() * 6 + 'a');//产生a-f
                    break;
                default:
                    break;
            }

            str = str + temp;
        }
        return str;
    }
}
