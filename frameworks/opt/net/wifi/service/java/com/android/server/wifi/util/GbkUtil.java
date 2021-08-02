package com.android.server.wifi.util;

import android.net.wifi.WifiSsid;
import android.util.Log;

import com.android.server.wifi.wificond.HiddenNetwork;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

public class GbkUtil {
    private GbkUtil() {}

    private static final String TAG = "GbkUtil";
    private static ArrayList<String> mGbkList = new ArrayList<>();

    // clear GBK history list
    public static void clear() {
        synchronized (mGbkList) {
            mGbkList.clear();
        }
    }

    // Check if a WifiSsid is GBK
    public static void checkAndSetGbk(WifiSsid ssid) {
        byte[] byteArray = ssid.octets.toByteArray();

        // set and add to list if GBK
        if (isNotUtf8(byteArray, 0, byteArray.length)) {
            Log.d(TAG, "SSID " + ssid.toString() + " is GBK encoding");
            ssid.mIsGbkEncoding = true;
            String str = ssid.toString();
            synchronized (mGbkList) {
                if (!mGbkList.contains(str)) {
                    mGbkList.add(str);
                }
            }
        }
    }

    // Check if a byte array is GBK
    public static boolean checkAndSetGbk(byte[] byteArray) {
        // set and add to list if GBK
        if (isNotUtf8(byteArray, 0, byteArray.length)) {
            WifiSsid ssid = WifiSsid.createFromByteArray(byteArray);
            Log.d(TAG, "SSID " + ssid.toString() + " is GBK encoding");
            ssid.mIsGbkEncoding = true;
            String str = ssid.toString();
            synchronized (mGbkList) {
                if (!mGbkList.contains(str)) {
                    mGbkList.add(str);
                }
            }
            return true;
        }
        return false;
    }

    // convert string to byte array list
    public static ArrayList<Byte> stringToByteArrayList(String str) {
        ArrayList<Byte> byteArrayList = new ArrayList<Byte>();

        try {
            for (byte b : str.getBytes("GBK")) {
                byteArrayList.add(new Byte(b));
            }
        } catch (UnsupportedEncodingException e) {
            Log.d(TAG, "UnsupportedEncodingException: " + e.toString());
        }
        return byteArrayList;
    }

    public static byte[] stringToByteArray(String str) {
        try {
            return str.getBytes("GBK");
        } catch (UnsupportedEncodingException e) {
            Log.d(TAG, "UnsupportedEncodingException: " + e.toString());
            return null;
        }
    }

    // check whether a string is GBK encoding
    public static boolean isGbkSsid(String ssid) {
        synchronized (mGbkList) {
            return mGbkList.contains(NativeUtil.removeEnclosingQuotes(ssid));
        }
    }

    //If SSID is not all ASCII (may be Chinese) and it isn't in GBK list,
    //then add a GBK SSID in hidden network list for scan
    public static HiddenNetwork needAddExtraGbkSsid(String ssid_original) {
        String ssid = NativeUtil.removeEnclosingQuotes(ssid_original);
        if (!isAllASCII(NativeUtil.decodeSsid(ssid_original)) && !isGbkSsid(ssid)) {
            HiddenNetwork network = new HiddenNetwork();
            try {
                network.ssid = NativeUtil.byteArrayFromArrayList(stringToByteArrayList(ssid));
            } catch (IllegalArgumentException e) {
                Log.e(TAG, "Illegal argument " + ssid, e);
                return null;
            }
            return network;
        }
        return null;
    }

    private static boolean isAllASCII(ArrayList<Byte> ssidByteArray) {
        if (ssidByteArray == null) {
            return false;
        }
        for (int i=0; i < ssidByteArray.size(); i++) {
            if (!isASCII(ssidByteArray.get(i))) {
                return false;
            }
        }
        return true;
    }

    private static boolean isNotUtf8(byte[] input, int ssidStartPos, int ssidEndPos) {
        int nBytes = 0, lastWildcar = 0;
        byte chr;
        boolean isAllAscii = true;
        boolean isAllGBK = true;
        boolean isWildcardChar = false;
        for (int i = ssidStartPos; i < ssidEndPos && i < input.length; i++) {
            chr = input[i];
            if (!isASCII(chr)) {
                isAllAscii = false;
                isWildcardChar = !isWildcardChar;
                if (isWildcardChar && i < input.length - 1) {
                    byte chr1 = input[i + 1];
                    if (!isGBKChar(chr, chr1)) {
                        isAllGBK = false;
                    }
                }
            } else {
                isWildcardChar = false;
            }
            if (0 == nBytes) {
                if ((chr & 0xFF) >= (0x80 & 0xFF)) {
                    lastWildcar = i;
                    nBytes = getUtf8CharLen(chr);
                    if (nBytes == 0) {
                        return true;
                    }
                    nBytes--;
                }
            } else {
                if ((chr & 0xC0) != 0x80) {
                    break;
                }
                nBytes--;
            }
        }
        //Log.d(TAG, "nBytes > 0: " + (nBytes > 0) + ", isAllAscii: " + isAllAscii);
        if (nBytes > 0) {
            if (isAllAscii) {
                return false;
            } else if (isAllGBK) {
                return true;
            } else {
                nBytes = getUtf8CharLen(input[lastWildcar]);
                for (int j = lastWildcar; j < (lastWildcar + nBytes) &&  j < input.length; j++) {
                    if (!isASCII(input[j])) {
                        input[j] = 0x20;
                    }
                }
                return false;
            }
        }
        return false;
    }

    private static int getUtf8CharLen(byte firstByte) {
        int nBytes = 0;
        if (firstByte >= (byte) 0xFC && firstByte <= (byte) 0xFD) {
            nBytes = 6;
        } else if (firstByte >= (byte) 0xF8) {
            nBytes = 5;
        } else if (firstByte >= (byte) 0xF0) {
            nBytes = 4;
        } else if (firstByte >= (byte) 0xE0) {
            nBytes = 3;
        } else if (firstByte >= (byte) 0xC0) {
            nBytes = 2;
        } else {
            return 0;
        }
        return nBytes;
    }

    private static boolean isASCII(byte b) {
        if ((b & 0x80) == 0) {
            return true;
        }
        return false;
    }

    private static boolean isGBKChar(byte head, byte tail) {
        int b0 = head & 0xff;
        int b1 = tail & 0xff;
        if ((b0 >= 0xA1 && b0 <= 0xA9 && b1 >= 0xA1 && b1 <= 0xFE) ||
            (b0 >= 0xB0 && b0 <= 0xF7 && b1 >= 0xA1 && b1 <= 0xFE) ||
            (b0 >= 0x81 && b0 <= 0xA0 && b1 >= 0x40 && b1 <= 0xFE) ||
            (b0 >= 0xAA && b0 <= 0xFE && b1 >= 0x40 && b1 <= 0xA0 && b1 != 0x7F) ||
            (b0 >= 0xA8 && b0 <= 0xA9 && b1 >= 0x40 && b1 <= 0xA0 && b1 != 0x7F) ||
            (b0 >= 0xAA && b0 <= 0xAF && b1 >= 0xA1 && b1 <= 0xFE && b1 != 0x7F) ||
            (b0 >= 0xF8 && b0 <= 0xFE && b1 >= 0xA1 && b1 <= 0xFE) ||
            (b0 >= 0xA1 && b0 <= 0xA7 && b1 >= 0x40 && b1 <= 0xA0 && b1 != 0x7F)) {
                return true;
            }
        return false;
    }
}
