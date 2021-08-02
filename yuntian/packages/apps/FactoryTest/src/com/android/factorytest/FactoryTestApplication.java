package com.android.factorytest;

import android.app.Application;

import com.android.factorytest.model.TestInfo;

import java.util.ArrayList;

/**
 * 工厂测试Application类
 * 功能：
 *      （１）初始化测试项及其状态
 *      （２）因为WIFI、BT和GPS在搜索设备或卫星的过程长，因此在打开工厂测试应用后
 *          马上打开WIFI、BT和GPS并立刻开始搜索设备和卫星，以节省工厂测试时间。
 */
public class FactoryTestApplication extends Application {

    private ArrayList<TestInfo> mTestInfoList;
    private ArrayList<Integer> mTestGroupList;

    @Override
    public void onCreate() {
        super.onCreate();
        mTestInfoList = Utils.initTestInfos(this);
        mTestGroupList = Utils.initTestGroupList(this, mTestInfoList);
    }

    public ArrayList<TestInfo> getTestInfoList() {
        if (mTestInfoList == null || mTestInfoList.isEmpty()) {
            mTestInfoList = Utils.initTestInfos(this);
        }
        return mTestInfoList;
    }

    public ArrayList<Integer> getTestGroupList() {
        if (mTestGroupList == null || mTestGroupList.isEmpty()) {
            mTestGroupList = Utils.initTestGroupList(this, getTestInfoList());
        }
        return mTestGroupList;
    }

    public void updateTestStates() {
        Utils.updateTestStates(this, getTestInfoList());
    }
}
