package com.mesh.test.provisioner.sqlite;

import android.content.ContentValues;
import android.database.Cursor;

import com.mesh.test.provisioner.adapter.UnprovisionedAdapter;

import java.util.Arrays;
import java.util.List;
import com.mesh.test.provisioner.util.MeshUtils;
import android.util.Log;


public class MyCallBack implements LouSQLite.ICallBack {

    public static final String TABLE_NAME_STORAGE = "storagedata"; //Table Name
    public static final String TABLE_NAME_NODE = "nodedata"; //Table Name

    private static final String DATABASE_NAME = "mesh.db"; //Database name
    private static final int DATABASE_VERSION = 1;
    private static final String TYPE_TEXT = " TEXT";
    private static final String TYPE_INTEGER = " INTEGER";
    private static final String SEP_COMMA = ",";

    private static final String TABLE_SCHEMA_STORAGE =
            "CREATE TABLE " + TABLE_NAME_STORAGE + " (" +
                    ProvisionedDeviceEntry._ID + TYPE_INTEGER + " PRIMARY KEY AUTOINCREMENT, " +
                    ProvisionedDeviceEntry.COLEUM_NAME_ID + TYPE_TEXT + SEP_COMMA +
                    ProvisionedDeviceEntry.COLEUM_NAME_NETKEY + TYPE_TEXT + SEP_COMMA +
                    ProvisionedDeviceEntry.COLEUM_NAME_APPKEY + TYPE_TEXT + SEP_COMMA +
                    ProvisionedDeviceEntry.COLEUM_NAME_GROUPADDR + TYPE_TEXT +
                    ")";

    private static final String TABLE_SCHEMA_NODE =
            "CREATE TABLE " + TABLE_NAME_NODE + " (" +
                    ProvisionedDeviceEntry._ID + TYPE_INTEGER + " PRIMARY KEY AUTOINCREMENT, " +
                    ProvisionedDeviceEntry.COLEUM_NAME_ADDRESS+ TYPE_TEXT + SEP_COMMA +
                    ProvisionedDeviceEntry.COLEUM_NAME_NODE + TYPE_TEXT +
                    ")";


    public MyCallBack() {
    }

    /**
     * Create a SLQ collection of tables
     */
    @Override
    public List<String> createTablesSQL() {
        return Arrays.asList(
                TABLE_SCHEMA_STORAGE,
                TABLE_SCHEMA_NODE
        );
    }

    /**
     * Get the name of the database
     */
    @Override
    public String getDatabaseName() {
        return DATABASE_NAME;
    }

    /**
     * Get the version of the database
     */
    @Override
    public int getVersion() {
        return DATABASE_VERSION;
    }


    /**
     * Store different data according to different tables
     */
    @Override
    public <T> void assignValuesByEntity(String tableName, T t, ContentValues values) {

        switch (tableName) {
            case TABLE_NAME_STORAGE:
                if (t instanceof StorageData) {
                    StorageData mStorageData = (StorageData) t;
                    values.put(ProvisionedDeviceEntry.COLEUM_NAME_ID, mStorageData.getId());
                    values.put(ProvisionedDeviceEntry.COLEUM_NAME_NETKEY, MeshUtils.NetKeyListToString(mStorageData.getAllNetKey()));
                    values.put(ProvisionedDeviceEntry.COLEUM_NAME_APPKEY, MeshUtils.AppKeyListToString(mStorageData.getAllAppKey()));
                    values.put(ProvisionedDeviceEntry.COLEUM_NAME_GROUPADDR, MeshUtils.GroupAddrListToString(mStorageData.getAllGroupAddr()));
                }
                break;
            case TABLE_NAME_NODE:
                if (t instanceof NodeData) {
                    NodeData mNodeData = (NodeData) t;
                    values.put(ProvisionedDeviceEntry.COLEUM_NAME_ADDRESS, mNodeData.getAddress());
                    values.put(ProvisionedDeviceEntry.COLEUM_NAME_NODE, MeshUtils.NodeToString(mNodeData.getNode()));
                }
                break;
        }
    }

    /**
     * Cursor to JavaBean
     * @param tableName
     * @param cursor
     * @return
     */
    @Override
    public Object newEntityByCursor(String tableName, Cursor cursor) {
        switch (tableName) {
            case TABLE_NAME_STORAGE:
                StorageData mStorageData = new StorageData();
                mStorageData.setId(cursor.getString(cursor.getColumnIndex(ProvisionedDeviceEntry.COLEUM_NAME_ID)));
                mStorageData.setAllNetKey(MeshUtils.StringToNetKeyList(cursor.getString(cursor.getColumnIndex(ProvisionedDeviceEntry.COLEUM_NAME_NETKEY))));
                mStorageData.setAllAppKey(MeshUtils.StringToAppKeyList(cursor.getString(cursor.getColumnIndex(ProvisionedDeviceEntry.COLEUM_NAME_APPKEY))));
                mStorageData.setAllGroupAddrList(MeshUtils.StringToGroupAddrList(cursor.getString(cursor.getColumnIndex(ProvisionedDeviceEntry.COLEUM_NAME_GROUPADDR))));
                return mStorageData;
            case TABLE_NAME_NODE:
                NodeData mNodeData = new NodeData();
                mNodeData.setAddress(cursor.getString(cursor.getColumnIndex(ProvisionedDeviceEntry.COLEUM_NAME_ADDRESS)));
                mNodeData.setNode(MeshUtils.StringToNode(cursor.getString(cursor.getColumnIndex(ProvisionedDeviceEntry.COLEUM_NAME_NODE))));
                return mNodeData;
        }

        return null;
    }


}
