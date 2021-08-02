package com.mesh.test.provisioner;

import android.app.Application;

import com.mesh.test.provisioner.model.Element;
import com.mesh.test.provisioner.sqlite.LouSQLite;
import com.mesh.test.provisioner.sqlite.MyCallBack;

import java.util.ArrayList;
import java.util.List;
import com.mesh.test.provisioner.sqlite.Node;

public class MyApplication extends Application {

    public ArrayList<Element> elements = new ArrayList<>();
    public static boolean isProvisioning = false;
    public static long provisioningTime = 0;
    public static long configTime = 0;
    public static long sendMessageTime = 0;
    public static long keyrefreshTime = 0;
    public static long groupSendMessageTime = 0;
    public static ArrayList<Node> nodeList = new ArrayList<>();
    public static int netkeyindex = 0;
    public static int appkeyindex = 0;
    public static int GattConnectStatus = 7;

    private static MyApplication myApplication = null;

    public static final int STORAGE_DATA_ALL = 0;
    //public static final int STORAGE_DATA_NODE = 1;
    public static final int STORAGE_DATA_NETKEY = 2;
    public static final int STORAGE_DATA_APPKEY = 3;
    public static final int STORAGE_DATA_GROUP_ADDR = 4;

    public static final int NODE_DATA_ADD = 10;
    public static final int NODE_DATA_DELETE = 11;
    public static final int NODE_DATA_UPDATE = 12;


    public static final int PB_GATT_CONNECT = 6;
    public static final int PB_GATT_DISCONNECT = 7;

    public static final int GROUP_SEND_MSG_START = 8;
    public static final int GROUP_SEND_MSG_END = 9;


    public static MyApplication getApplication() {
        return myApplication;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        // init SQLite
        LouSQLite.init(this.getApplicationContext(), new MyCallBack());
        myApplication = this;
    }
}
