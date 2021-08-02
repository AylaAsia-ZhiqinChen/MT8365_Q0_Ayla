package com.android.factorytest.model;

import android.content.ComponentName;
import android.content.Intent;
import android.os.Parcel;
import android.os.Parcelable;

/**
 * 测试项信息
 */
public class TestInfo implements Parcelable {

    /**
     * 测试应用包名
     */
    private String mPackageName;
    /**
     * 测试项类名
     */
    private String mClassName;
    /**
     * 测试项状态
     */
    private TestState mState;
    /**
     * 测试项所属测试组
     */
    private int mGroupResId;
    /**
     * 测试名称
     */
    private int mTestTitleResId;
    /**
     * 测试所属测试组的优先级
     */
    private int mGroupPriority;
    /**
     * 测试项在测试组中的优先级
     */
    private int mPriority;
    /**
     * 测试项在所有测试中的位置
     */
    private int mOrder;

    public TestInfo(String packageName, String className, TestState state, int groupResId,
                    int testTitleResId, int groupPriority, int priority, int order) {
        mPackageName = packageName;
        mClassName = className;
        mState = state;
        mGroupResId = groupResId;
        mTestTitleResId = testTitleResId;
        mGroupPriority = groupPriority;
        mPriority = priority;
        mOrder = order;
    }

    public TestInfo(Parcel parcel) {
        mPackageName = parcel.readString();
        mClassName = parcel.readString();
        mState = TestState.valueOf(parcel.readString());
        mGroupResId = parcel.readInt();
        mTestTitleResId = parcel.readInt();
        mGroupPriority = parcel.readInt();
        mPriority = parcel.readInt();
        mOrder = parcel.readInt();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(mPackageName);
        dest.writeString(mClassName);
        dest.writeString(mState.name());
        dest.writeInt(mGroupResId);
        dest.writeInt(mTestTitleResId);
        dest.writeInt(mGroupPriority);
        dest.writeInt(mPriority);
        dest.writeInt(mOrder);
    }

    public static final Parcelable.Creator<TestInfo> CREATOR = new Creator<TestInfo>() {
        @Override
        public TestInfo createFromParcel(Parcel source) {
            return new TestInfo(source);
        }

        @Override
        public TestInfo[] newArray(int size) {
            return new TestInfo[size];
        }
    };

    public String getPackageName() {
        return mPackageName;
    }

    public String getClassName() {
        return mClassName;
    }

    public TestState getState() {
        return mState;
    }

    public void setTestState(TestState state) {
        mState = state;
    }

    public int getGroupResId() {
        return mGroupResId;
    }

    public int getTestTitleResId() {
        return mTestTitleResId;
    }

    public int getGroupPriority() {
        return mGroupPriority;
    }

    public int getPriority() {
        return mPriority;
    }

    public int getOrder() {
        return mOrder;
    }

    public void setOrder(int order) {
        mOrder = order;
    }

    public Intent getIntent() {
        Intent intent = new Intent();
        ComponentName cn = new ComponentName(mPackageName, mClassName);
        intent.setComponent(cn);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        return intent;
    }

    @Override
    public String toString() {
        return "[" + mPackageName + "," + mClassName + "," + mGroupResId + "," + mTestTitleResId + "," +
                "" + "" + mGroupPriority + "," + mPriority + "," + mOrder + "]";

    }
}
