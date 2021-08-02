package com.android.factorytest;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import com.android.factorytest.model.TestInfo;
import com.android.factorytest.model.TestState;

import java.util.ArrayList;

/**
 * 工厂测试应用数据库帮助类，主要用于存储测试结果
 */
public class FactoryTestDatabase {

    // 数据库名称
    private static final String DATABASE_NAME = "factory_test.db";
    // 数据库版本号
    private static final int DATABASE_VERSION = 1;

    // 测试结果表名称
    public static final String TEST_RESULT_TABLE = "test_result";
    // ID
    public static final String _ID = "_id";
    // 测试项名称资源id
    public static final String TITLE_ID = "title_id";
    // 测试结果
    public static final String RESULT = "result";
    // 测试项排序
    public static final String ORDER = "test_order";

    // 测试结果表栏目数组
    public static final String[] COLUMNS = {_ID, TITLE_ID, RESULT, ORDER};

    private Context mContext;

    private static FactoryTestDatabase mFactoryTestDatabase;
    private FactoryTestOpenHelper mHelper;

    public static final FactoryTestDatabase getInstance(Context context) {
        if (mFactoryTestDatabase == null) {
            mFactoryTestDatabase = new FactoryTestDatabase(context);
        }
        return mFactoryTestDatabase;
    }

    private FactoryTestDatabase(Context context) {
        mContext = context;
        mHelper = new FactoryTestOpenHelper(context);
    }

    /**
     * 获取测试项的测试状态
     * @param titleResId　测试项标题资源ID
     * @return 返回该测试项多测试状态，如果没有找到该测试项多记录，则返回TestState.UNKNOWN.
     */
    public TestState getTestState(int titleResId) {
        SQLiteDatabase sb = mHelper.getReadableDatabase();
        TestState state = TestState.UNKNOWN;
        String[] cols = new String[]{TITLE_ID, RESULT};
        String[] selectionArgs = new String[]{titleResId + ""};
        Cursor c = sb.query(TEST_RESULT_TABLE, cols, TITLE_ID + "=?", selectionArgs, null, null, null);
        if (c != null) {
            if (c.getCount() > 0) {
                c.moveToFirst();
                String s = c.getString(c.getColumnIndexOrThrow(RESULT));
                state = TestState.valueOf(s);
                Log.i(this, "getTestState=>" + mContext.getString(titleResId) + " test state is " + s);
            } else {
                Log.e(this, "getTestState=>" + mContext.getString(titleResId) + " is not found.");
            }
            c.close();
        } else {
            Log.e(this, "getTestState=>query return null.");
        }
        return state;
    }

    /**
     * 设置测试结果
     * @param item 要设置测试状态的测试项信息
     * @return 返回更新结果，如果更新成功返回true，否则返回false
     */
    public boolean setTestState(TestInfo item) {
        long result = -1L;
        SQLiteDatabase sd = mHelper.getWritableDatabase();
        String[] cols = new String[]{TITLE_ID, RESULT};
        String[] selectionArgs = new String[]{item.getTestTitleResId() + ""};
        ContentValues cv = new ContentValues();
        cv.put(TITLE_ID, item.getTestTitleResId());
        cv.put(RESULT, item.getState().name());
        cv.put(ORDER, item.getOrder());
        Cursor c = sd.query(TEST_RESULT_TABLE, cols, TITLE_ID + "=?", selectionArgs, null, null, null);
        if (c != null && c.getCount() > 0) {
            Log.i(this, "setTestState=>update " + mContext.getString(item.getTestTitleResId()) + " test state to " + item.getState());
            c.close();
            String[] whereArgs = new String[]{item.getTestTitleResId() + ""};
            result = sd.update(TEST_RESULT_TABLE, cv, TITLE_ID + "=?", whereArgs);
        } else {
            Log.i(this, "setTestState=>inser " + mContext.getString(item.getTestTitleResId()) + " test state to " + item.getState());
            c.close();
            result = sd.insert(TEST_RESULT_TABLE, null, cv);
        }
        Log.i(this, "setTestState=>resutl: " + result);
        return (result != -1 ? true : false);
    }

    /**
     * 获取所有测试项的测试状态
     * @return 返回所有测试项的测试状态集合
     */
    public ArrayList<TestResultItem> getAllTestStates() {
        ArrayList<TestResultItem> list = new ArrayList<TestResultItem>();
        SQLiteDatabase sb = mHelper.getReadableDatabase();
        Cursor c = sb.query(TEST_RESULT_TABLE, COLUMNS, null, null, null, null, ORDER  + " ASC");
        if (c != null) {
            if (c.getCount() > 0) {
                int titleResId = -1;
                int s = 0;
                int order = -1;
                String state = TestState.UNKNOWN.toString();
                while (c.moveToNext()) {
                    titleResId = c.getInt(c.getColumnIndexOrThrow(TITLE_ID));
                    state = c.getString(c.getColumnIndexOrThrow(RESULT));
                    order = c.getInt(c.getColumnIndexOrThrow(ORDER));
                    TestResultItem item = new TestResultItem();
                    item.mTitleResId = titleResId;
                    item.mState = TestState.valueOf(state);
                    item.mOrder = order;
                    list.add(item);
                }
            } else {
                Log.e(this, "getAllTestStates=>no test state.");
            }
            c.close();
        } else {
            Log.e(this, "getAllTestStates=>query return null.");
        }
        Log.i(this, "getAllTestStates=>size: " + list.size());
        return list;
    }

    /**
     * 清除数据库中所有数据
     */
    public void clearDatabase() {
        SQLiteDatabase sd = mHelper.getWritableDatabase();
        sd.execSQL("delete from " + TEST_RESULT_TABLE + ";");
        sd.execSQL("update sqlite_sequence SET seq = 0 where name =\'" + TEST_RESULT_TABLE + "\';");
    }

    class FactoryTestOpenHelper extends SQLiteOpenHelper {

        public FactoryTestOpenHelper(Context context) {
            super(context, DATABASE_NAME, null, DATABASE_VERSION);
        }

        @Override
        public void onCreate(SQLiteDatabase sqLiteDatabase) {
            sqLiteDatabase.execSQL("CREATE TABLE " + TEST_RESULT_TABLE
                    + " (" + _ID + " INTEGER PRIMARY KEY AUTOINCREMENT,"
                    + TITLE_ID + " INTEGER,"
                    + RESULT + " TEXT,"
                    + ORDER + " INTEGER);");
        }

        @Override
        public void onUpgrade(SQLiteDatabase sqLiteDatabase, int i, int i1) {
            sqLiteDatabase.execSQL("DROP TABLE IF EXISTS " + TEST_RESULT_TABLE);
            onCreate(sqLiteDatabase);
        }
    }

    static class TestResultItem {
        public int mOrder;
        public int mTitleResId;
        public TestState mState;
    }
}
