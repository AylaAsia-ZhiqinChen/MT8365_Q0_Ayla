package com.mesh.test.provisioner.util;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import com.mesh.test.provisioner.model.Element;
import java.lang.reflect.Type;
import java.util.ArrayList;
import com.mesh.test.provisioner.sqlite.Node;
import com.mesh.test.provisioner.NetKey;
import com.mesh.test.provisioner.AppKey;
import java.text.DecimalFormat;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import android.content.res.AssetManager;
import android.text.TextUtils;
import android.content.Context;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.Properties;
import com.mesh.test.provisioner.SafeProperties;




public class MeshUtils {

    public static String NodeToString(Node node) {
        Gson gson = new Gson();
        String str= gson.toJson(node);
        return str;
    }

    public static Node StringToNode(String str) {
        Gson gson = new Gson();
        Node node = gson.fromJson(str, Node.class);
        return node;
    }


    public static String NodeListToString(ArrayList<Node> array) {
        CopyOnWriteArrayList<Node> copyList = new CopyOnWriteArrayList<>(array);
        Gson gson = new Gson();
        String str = gson.toJson(copyList);
        return str;
    }

    public static ArrayList<Node> StringToNodeList(String str) {
        Gson gson = new Gson();
        Type type = new TypeToken<ArrayList<Node>>() {}.getType();
        ArrayList<Node> array = gson.fromJson(str, type);
        return array;
    }

    public static String NetKeyListToString(ArrayList<NetKey> array) {
        Gson gson = new Gson();
        String str= gson.toJson(array);
        return str;
    }

    public static ArrayList<NetKey> StringToNetKeyList(String str) {
        Gson gson = new Gson();
        Type type = new TypeToken<ArrayList<NetKey>>() {}.getType();
        ArrayList<NetKey> array = gson.fromJson(str, type);
        return array;
    }

    public static String AppKeyListToString(ArrayList<AppKey> array) {
        Gson gson = new Gson();
        String str= gson.toJson(array);
        return str;
    }

    public static ArrayList<AppKey> StringToAppKeyList(String str) {
        Gson gson = new Gson();
        Type type = new TypeToken<ArrayList<AppKey>>() {}.getType();
        ArrayList<AppKey> array = gson.fromJson(str, type);
        return array;
    }

    public static String GroupAddrListToString(ArrayList<Integer> array) {
        Gson gson = new Gson();
        String str= gson.toJson(array);
        return str;
    }

    public static ArrayList<Integer> StringToGroupAddrList(String str) {
        Gson gson = new Gson();
        Type type = new TypeToken<ArrayList<Integer>>() {}.getType();
        ArrayList<Integer> array = gson.fromJson(str, type);
        return array;
    }

    public static String ArrayToString(int[] array) {
        Gson gson = new Gson();
        String str= gson.toJson(array);
        return str;
    }

    public static int[] StringToArray(String str) {
        Gson gson = new Gson();
        Type type = new TypeToken<int[]>() {}.getType();
        int[] array = gson.fromJson(str, type);
        return array;
    }

    public static String intArrayToString(int[] intarray, boolean uuid) {
        if(null == intarray) {
            return null;
        }
        StringBuffer str = new StringBuffer();
        for (int i = 0; i < intarray.length ; i++) {
            str = str.append(String.format("%02x", intarray[i]));
            if (uuid && (i == 3 || i == 7 || i == 11)) {
                str = str.append(" - ");
            } else if (i < (intarray.length - 1)) {
                str = str.append(" ");
            }
        }
        return str.toString();
    }

    public static int[] StringToIntArray(String str) {
        if(null == str) {
            return null;
        }
        String[] strArray = str.split("_");
        int[] intArray = new int[strArray.length];
        for(int i = 0; i < strArray.length; i++) {
            try {
                intArray[i] = Integer.parseInt(strArray[i], 16);
            } catch(Exception e) {
                return null;
            }
        }
        return intArray;
    }

    public static double getDoubleDecimal(double num) {
        DecimalFormat df = new DecimalFormat("#.000");
        String str = df.format(num);
        Double temp = Double.valueOf(str);
        return temp;
    }

    public static String decimalToHexString(String format, long number) {
        return new StringBuffer("0x").append(String.format(format, number)).toString();
    }

    public static String[] decimalArrToHexArr(int[] arr) {
        String[] temp = new String[arr.length];
        for(int i = 0; i < arr.length; i++) {
            temp[i] = String.format("%02x", arr[i]);
        }
        return temp;
    }

    public static long hexSrtingToDecimal(String str) {
        long result = -1;
        try {
            result = Integer.parseInt(str.substring(2),16);
        } catch(Exception e) {
            result = -1;
        }
        return result;
    }

    private static boolean isMatch(String regex, String orginal){
        if (orginal == null || orginal.trim().equals("")) {
            return false;
        }
        Pattern pattern = Pattern.compile(regex);
        Matcher isNum = pattern.matcher(orginal);
        return isNum.matches();
    }

    public static boolean isPositiveInteger(String orginal) {
        return isMatch("^\\+{0,1}[1-9]\\d*", orginal);
    }

    public static boolean isHexNumber(String orginal) {
        return isMatch("^[A-Fa-f0-9]{4}$", orginal);
    }

    public static boolean isFileExisted(String filePath) {
        if (new File(filePath).exists()) {
            return true;
        }
        return false;
    }

    public static void writeProperties(String filePath, String pKey, String pValue) {
        InputStream in = null;
        OutputStream out = null;
        try {
            SafeProperties pps = new SafeProperties();
            in = new FileInputStream(filePath);
            pps.load(in);
            out = new FileOutputStream(filePath);
            pps.setProperty(pKey, pValue);
            pps.store(out, null);
        } catch(Exception e) {
            e.printStackTrace();
        } finally {
            if (out != null) {
                try {
                    out.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public  static String readFileFromAssets(Context context, String fileName) throws IOException, IllegalArgumentException {
        if (null == context || TextUtils.isEmpty( fileName )){
            throw new IllegalArgumentException( "bad arguments!" );
        }

        AssetManager assetManager = context.getAssets();
        InputStream input = assetManager.open(fileName);
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        byte[] buffer = new byte[1024];
        int length = 0;
        while ((length = input.read(buffer)) != -1) {
            output.write(buffer, 0, length);
        }
        output.close();
        input.close();

        return output.toString();
    }

    public static void save(String filePath,String content) {
        BufferedWriter bw = null;
        FileOutputStream fos = null;
        try {
            File file = new File(filePath);
            fos = new FileOutputStream(file);
            bw = new BufferedWriter(new OutputStreamWriter(fos));
            bw.write(content);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (bw != null) {
                try {
                    bw.flush();
                    bw.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

        }
    }

}
