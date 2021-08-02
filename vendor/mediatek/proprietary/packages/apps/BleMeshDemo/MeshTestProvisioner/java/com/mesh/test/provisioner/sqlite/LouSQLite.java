package com.mesh.test.provisioner.sqlite;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.ArrayList;
import java.util.List;
import android.util.Log;

public final class LouSQLite extends SQLiteOpenHelper {

    private static final String TAG = "LouSQLite";
    private AtomicInteger mOpenCounter = new AtomicInteger();
    private static LouSQLite INSTANCE;
    private final ICallBack callBack;
    private SQLiteDatabase mDatabase;


    private LouSQLite(Context context, ICallBack callBack) {
        super(context, callBack.getDatabaseName(), null, callBack.getVersion());
        this.callBack = callBack;
    }

    /**
     * Initialize the database
     * @param context
     * @param callBack
     */
    public static void init(Context context, ICallBack callBack) {
        INSTANCE = new LouSQLite(context, callBack);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        List<String> tablesSQL = callBack.createTablesSQL();
        for (String create_table : tablesSQL) {
            db.execSQL(create_table);
            Log.i(TAG, "create table " + "[ \n" + create_table + "\n ]" + " successful");
        }
    }

    /**
     * Create a table
     * @param tableName
     */
    public void createTable(String tableName) {
        SQLiteDatabase db = INSTANCE.getWritableDatabase();
        db.beginTransaction();
        try {
            db.execSQL(tableName);
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
            db.close();
        }
    }

    @Override
    public void onUpgrade(SQLiteDatabase sqLiteDatabase, int oldVersion, int newVersion) {
    }


    /**
     * Insert a single piece of data
     */
    public static <T> void insert(String tableName, T entity) {
        SQLiteDatabase db = INSTANCE.openDataBase();
        db.beginTransaction();
        try {
            ContentValues values = new ContentValues();
            //Put the data into ContentValues
            INSTANCE.callBack.assignValuesByEntity(tableName, entity, values);
            //Insert data into the database
            long index = db.insert(tableName, null, values);
            if(index > 0) {
                Log.i(TAG, tableName + " insert success");
            }else {
                Log.i(TAG, tableName + " insert fail");
            }
            values.clear();
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
            INSTANCE.closeDatabase();
        }
    }


    /**
     * Insert data collection
     */
    public static <T> void insert(String tableName, List<T> entities) {
        SQLiteDatabase db = INSTANCE.openDataBase();
        db.beginTransaction();
        try {
            ContentValues values = new ContentValues();
            for (T entity : entities) {
                INSTANCE.callBack.assignValuesByEntity(tableName, entity, values);
                db.insert(tableName, null, values);
                values.clear();
            }
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
            INSTANCE.closeDatabase();
        }
    }

    /**
     * change the data
     */
    public static <T> void update(String tableName, T entity, String whereClause, String[] whereArgs) {
        SQLiteDatabase db = INSTANCE.openDataBase();
        db.beginTransaction();
        try {
            ContentValues values = new ContentValues();
            INSTANCE.callBack.assignValuesByEntity(tableName, entity, values);
            db.update(tableName, values, whereClause, whereArgs);
            values.clear();
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
            INSTANCE.closeDatabase();
        }
    }


    /**
     * Query data, return a List collection
     */
    public static <T> List<T> query(String tableName, String queryStr, String[] whereArgs) {
        SQLiteDatabase db = INSTANCE.openDataBase();
        Cursor cursor = db.rawQuery(queryStr, whereArgs);
        try {
            List<T> lists = new ArrayList<>(cursor.getCount());
            if (cursor.moveToFirst()) {
                do {
                    T entity = INSTANCE.callBack.newEntityByCursor(tableName, cursor);
                    if (entity != null) {
                        lists.add(entity);
                    }
                } while (cursor.moveToNext());
            }
            return lists;
        } finally {
            cursor.close();
            INSTANCE.closeDatabase();
        }

    }

    /**
     * Delete all data in the table
     */
    public static void deleteFrom(String tableName) {

        SQLiteDatabase db = INSTANCE.openDataBase();
        db.beginTransaction();
        try {
            String sql = "DELETE FROM " + tableName;
            db.execSQL(sql);
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
            INSTANCE.closeDatabase();
        }
    }

    public static void delete(String tableName, String whereClause, String[] whereArgs) {
        SQLiteDatabase db = INSTANCE.openDataBase();
        db.beginTransaction();
        try {
            db.delete(tableName, whereClause, whereArgs);
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
            INSTANCE.closeDatabase();
        }
    }

    public static void execSQL(String sql) {
        SQLiteDatabase db = INSTANCE.openDataBase();
        db.beginTransaction();
        try {
            db.execSQL(sql);
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
            INSTANCE.closeDatabase();
        }
    }


    public synchronized SQLiteDatabase openDataBase() {
        if(mOpenCounter.incrementAndGet() == 1) {
            // Opening new database
            mDatabase = INSTANCE.getWritableDatabase();
        }
        return mDatabase;
    }


    public synchronized void closeDatabase() {

        if(mOpenCounter.decrementAndGet() == 0) {
            // Closing database
            mDatabase.close();
        }
    }


    /**
     * Callback interface
     */
    public interface ICallBack {
        String getDatabaseName();

        int getVersion();

        List<String> createTablesSQL();

        <T> void assignValuesByEntity(String tableName, T entity, ContentValues values);

        <T> T newEntityByCursor(String tableName, Cursor cursor);
    }
}
