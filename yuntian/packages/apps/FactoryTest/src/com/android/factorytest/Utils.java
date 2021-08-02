package com.android.factorytest;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.res.Resources;
import android.os.Bundle;

import com.android.factorytest.model.TestInfo;
import com.android.factorytest.model.TestState;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * 工厂测试工具类
 */
public class Utils {

    private static final String TAG = "Utils";

    /**
     * Group Res id 的Intent Extra键值
     * int值
     */
    public static final String EXTRA_GROUP_RES_ID = "group_res_id";
    /**
     * 自动测试下标Extra键值
     * int值
     */
    public static final String EXTRA_TEST_INDEX = "auto_test_index";
    /**
     * 是否是自动测试Extra键值
     * boolean值
     */
    public static final String EXTRA_AUTO_TEST = "auto_test";

    // 测试项Action
    private static final String TEST_ACTION = "android.intent.action.FACTORY_TEST";
    // 测试集合项名称
    private static final String GROUP_META_DATA_NAME = "com.android.factorytest.group";
    // 测试项名称
    private static final String TEST_TITLE_META_DATA_NAME = "com.android.factorytest.title";
    // 测试项在测试组中的优先级
    private static final String TEST_PRIORITY_META_DATA_NAME = "com.android.factorytest.priority";
    // 测试集合优先级
    private static final String GROUP_PRIORITY_META_DATA_NAME = "com.android.factorytest.group_priority";
    // 测试项状态
    private static final String TEST_ENABLED_META_DATA_NAME = "com.android.factorytest.enabled";

    /**
     * 初始化所有测试项
     *
     * @param context 　Context对象
     * @return 返回需要测试的测试项集合
     */
    public static ArrayList<TestInfo> initTestInfos(Context context) {
        ArrayList<TestInfo> testInfoList = new ArrayList<TestInfo>();
        Resources res = context.getResources();
        PackageManager pm = context.getPackageManager();
        Intent testIntent = new Intent(TEST_ACTION);
        testIntent.setPackage(context.getPackageName());
        // PackageManager.MATCH_DISABLED_COMPONENTS用于查找components　disable的Activity，比如用于测试NFC的测试项
        List<ResolveInfo> activities = pm.queryIntentActivities(testIntent,
                PackageManager.GET_META_DATA | PackageManager.GET_DISABLED_COMPONENTS);
        if (activities != null) {
            Log.i(TAG, "initTestInfos=>activities size: " + activities.size());
            Bundle metaData = null;
            for (ResolveInfo ri : activities) {
                metaData = ri.activityInfo.metaData;
                if (metaData != null && metaData.containsKey(GROUP_META_DATA_NAME)
                        && metaData.containsKey(TEST_TITLE_META_DATA_NAME)
                        && metaData.containsKey(GROUP_PRIORITY_META_DATA_NAME)
                        && metaData.containsKey(TEST_ENABLED_META_DATA_NAME)
                        && metaData.containsKey(TEST_PRIORITY_META_DATA_NAME)) {
                    boolean enabled = res.getBoolean(metaData.getInt(TEST_ENABLED_META_DATA_NAME));
                    if (enabled) {
                        //Log.d(TAG, "initTestInfos=>name: " + ri.activityInfo.name + " priority: " + res.getInteger(metaData.getInt(TEST_PRIORITY_META_DATA_NAME)));
                        TestInfo info = new TestInfo(ri.activityInfo.packageName,
                                ri.activityInfo.name,
                                TestState.UNKNOWN,
                                metaData.getInt(GROUP_META_DATA_NAME),
                                metaData.getInt(TEST_TITLE_META_DATA_NAME),
                                res.getInteger(metaData.getInt(GROUP_PRIORITY_META_DATA_NAME)),
                                res.getInteger(metaData.getInt(TEST_PRIORITY_META_DATA_NAME)),
                                -1);
                        testInfoList.add(info);
                    } else {
                        Log.e(TAG, "initTestInfos=>" + ri.activityInfo.name + " is disabled.");
                    }

                } else {
                    Log.e(TAG, "initTestInfos=>" + ri.activityInfo.name + " is not meet specifications.");
                }
            }
        }

        // 过滤掉不适用的测试项
        filterTestList(context, testInfoList);
        // 排序测试项
        Collections.sort(testInfoList, mTestInfoComparable);
        // 设置测试项在所有测试项中的位置
        for (int i = 0; i < testInfoList.size(); i++) {
            testInfoList.get(i).setOrder(i);
        }
        Log.i(TAG, "initTestInfos=>test size: " + testInfoList.size());
        return testInfoList;
    }

    /**
     * 过滤掉不适用的测试项
     *
     * @param context 　Context对象
     * @param list    　所有测试项包括不适用测试项
     */
    public static void filterTestList(Context context, ArrayList<TestInfo> list) {
        Resources res = context.getResources();
        filterTestItem(context, list, R.array.system_version_test_groups, res.getBoolean(R.bool.enabled_system_version_test));
        filterTestItem(context, list, R.array.rf_cali_test_groups, res.getBoolean(R.bool.enabled_rf_cali_test));
        filterTestItem(context, list, R.array.rtc_test_groups, res.getBoolean(R.bool.enabled_rtc_test));
        filterTestItem(context, list, R.array.screen_color_test_groups, res.getBoolean(R.bool.enabled_screen_color_test));
        filterTestItem(context, list, R.array.screen_back_light_test_groups, res.getBoolean(R.bool.enabled_screen_back_light_test));
        filterTestItem(context, list, R.array.single_finger_touch_test_groups, res.getBoolean(R.bool.enabled_single_finger_touch_test));
        filterTestItem(context, list, R.array.multi_finger_touch_test_groups, res.getBoolean(R.bool.enabled_mulit_finger_touch_test));
        filterTestItem(context, list, R.array.motor_test_groups, res.getBoolean(R.bool.enabled_motor_test));
        filterTestItem(context, list, R.array.speaker_test_groups, res.getBoolean(R.bool.enabled_speaker_test));
        filterTestItem(context, list, R.array.micro_phone_echo_test_groups, res.getBoolean(R.bool.enabled_micro_phone_echo_test));
        filterTestItem(context, list, R.array.sim_card_test_groups, res.getBoolean(R.bool.enabled_sim_card_test));
        filterTestItem(context, list, R.array.psam_card_test_groups, res.getBoolean(R.bool.enabled_psam_card_test));
        filterTestItem(context, list, R.array.handset_test_groups, res.getBoolean(R.bool.enabled_handset_test));
        filterTestItem(context, list, R.array.call_test_groups, res.getBoolean(R.bool.enabled_call_test));
        filterTestItem(context, list, R.array.mobile_data_test_groups, res.getBoolean(R.bool.enabled_mobile_data_test));
        filterTestItem(context, list, R.array.sdcard_test_groups, res.getBoolean(R.bool.enabled_sdcard_test));
        filterTestItem(context, list, R.array.charge_test_groups, res.getBoolean(R.bool.enabled_charge_test));
        filterTestItem(context, list, R.array.headset_test_groups, res.getBoolean(R.bool.enabled_headset_test));
        filterTestItem(context, list, R.array.typec_headset_test_groups, res.getBoolean(R.bool.enabled_typec_headset_test));
        filterTestItem(context, list, R.array.typec_digital_headset_test_groups, res.getBoolean(R.bool.enabled_typec_digital_headset_test));
        filterTestItem(context, list, R.array.typec_analog_headset_test_groups, res.getBoolean(R.bool.enabled_typec_analog_headset_test));
        filterTestItem(context, list, R.array.otg_test_groups, res.getBoolean(R.bool.enabled_otg_test));
        filterTestItem(context, list, R.array.dual_microphone_test_groups, res.getBoolean(R.bool.enabled_dual_microphone_test));
        filterTestItem(context, list, R.array.led_test_groups, res.getBoolean(R.bool.enabled_led_test));
        filterTestItem(context, list, R.array.scan_test_groups, res.getBoolean(R.bool.enabled_scan_test));
        filterTestItem(context, list, R.array.safemodule_test_groups, res.getBoolean(R.bool.enabled_safemodule_test));
        filterTestItem(context, list, R.array.serialport_test_groups, res.getBoolean(R.bool.enabled_serialport_test));
        filterTestItem(context, list, R.array.msix_headset_test_groups, res.getBoolean(R.bool.enabled_msix_headset_test));
        filterTestItem(context, list, R.array.serialport_simple_test_groups, res.getBoolean(R.bool.enabled_serialport_simple_test));
        filterTestItem(context, list, R.array.fmradio_test_groups, res.getBoolean(R.bool.enabled_fmradio_test));
        filterTestItem(context, list, R.array.dmr_test_groups, res.getBoolean(R.bool.enabled_dmr_test));
        filterTestItem(context, list, R.array.wifi_test_groups, res.getBoolean(R.bool.enabled_wifi_test));
        filterTestItem(context, list, R.array.bluetooth_test_groups, res.getBoolean(R.bool.enabled_bluetooth_test));
        filterTestItem(context, list, R.array.gps_test_groups, res.getBoolean(R.bool.enabled_gps_test));
		filterTestItem(context, list, R.array.ble_test_groups, res.getBoolean(R.bool.enabled_ble_test));
        filterTestItem(context, list, R.array.gravity_sensor_test_groups, res.getBoolean(R.bool.enabled_gravity_sensor_test));
        filterTestItem(context, list, R.array.proximity_sensor_test_groups, res.getBoolean(R.bool.enabled_proximity_sensor_test));
        filterTestItem(context, list, R.array.light_sensor_test_groups, res.getBoolean(R.bool.enabled_light_sensor_test));
        filterTestItem(context, list, R.array.fingerprint_sensor_test_groups, res.getBoolean(R.bool.enabled_fingerprint_sensor_test));
        filterTestItem(context, list, R.array.nfc_sensor_test_groups, res.getBoolean(R.bool.enabled_nfc_sensor_test));
        filterTestItem(context, list, R.array.accelerometer_sensor_test_groups, res.getBoolean(R.bool.enabled_accelerometer_sensor_test));
        filterTestItem(context, list, R.array.magnetic_fieled_sensor_test_groups, res.getBoolean(R.bool.enabled_magnetic_fieled_sensor_test));
        filterTestItem(context, list, R.array.pressure_sensor_test_groups, res.getBoolean(R.bool.enabled_pressure_sensor_test));
        filterTestItem(context, list, R.array.gyroscope_sensor_test_groups, res.getBoolean(R.bool.enabled_gyroscope_sensor_test));
        filterTestItem(context, list, R.array.step_count_sensor_test_groups, res.getBoolean(R.bool.enabled_step_count_sensor_test));
        filterTestItem(context, list, R.array.front_camera_test_groups, res.getBoolean(R.bool.enabled_front_camera_test));
        filterTestItem(context, list, R.array.back_camera_test_groups, res.getBoolean(R.bool.enabled_back_camera_test));
        filterTestItem(context, list, R.array.iris_camera_test_groups, res.getBoolean(R.bool.enabled_iris_camera_test));
        filterTestItem(context, list, R.array.electric_torch_test_groups, res.getBoolean(R.bool.enabled_electric_torch_test));
        filterTestItem(context, list, R.array.scan_camera_test_groups, res.getBoolean(R.bool.enabled_scan_camera_test));
        filterTestItem(context, list, R.array.key_test_groups, res.getBoolean(R.bool.enabled_key_test));
        filterTestItem(context, list, R.array.face_test_groups, res.getBoolean(R.bool.enabled_face_test));
        filterTestItem(context, list, R.array.lock_test_groups, res.getBoolean(R.bool.enabled_lock_test));
        filterTestItem(context, list, R.array.finger_test_groups, res.getBoolean(R.bool.enabled_finger_test));
        filterTestItem(context, list, R.array.test_result_groups, res.getBoolean(R.bool.enabled_test_result));
    }

    /**
     * 根据测试项是否适用来过滤掉该测试项
     *
     * @param context     　Context对象
     * @param list        　所有测试项集合
     * @param filterResId 　要过滤掉的测试项数组的资源ID
     * @param enabled     该测试项是否适用
     */
    private static void filterTestItem(Context context, ArrayList<TestInfo> list, int
            filterResId, boolean enabled) {
        Resources res = context.getResources();
        if (!enabled) {
            String[] testGroups = res.getStringArray(filterResId);
            for (String test : testGroups) {
                for (int i = 0; i < list.size(); i++) {
                    if (test.equals(list.get(i).getClassName())) {
                        Log.i(TAG, "filterTestItem=>" + list.get(i).getClassName() + " test does not apply, remove it.");
                        list.remove(i);
                        break;
                    }
                }
            }
        }
    }

    /**
     * 获取测试集合的标题资源ID集合
     *
     * @param context 　Context对象
     * @param list    　所有适用工厂测试集合
     * @return 返回测试集合标题的资源ID集合
     */
    public static ArrayList<Integer> initTestGroupList(Context context, ArrayList<TestInfo> list) {
        ArrayList<Integer> testGroupList = new ArrayList<Integer>();
        if (list != null && list.size() > 0) {
            int lastGroupResId = list.get(0).getGroupResId();
            testGroupList.add(lastGroupResId);
            for (TestInfo info : list) {
                int groupResId = info.getGroupResId();
                if (groupResId != lastGroupResId) {
                    lastGroupResId = groupResId;
                    testGroupList.add(groupResId);
                }
            }
            Log.i(TAG, "initTestGroupList=>size: " + testGroupList.size());
        } else {
            Log.e(TAG, "initTestGroupList=>list is null or empty.");
        }
        return testGroupList;
    }

    /**
     * 更新所有测试项多测试状态
     *
     * @param context 　Context对象
     * @param list    　要更新的测试项集合
     */
    public static void updateTestStates(Context context, ArrayList<TestInfo> list) {
        FactoryTestDatabase ftd = FactoryTestDatabase.getInstance(context);
        if (list != null && list.size() > 0) {
            for (int i = 0; i < list.size(); i++) {
                TestState state = ftd.getTestState(list.get(i).getTestTitleResId());
                list.get(i).setTestState(state);
            }
        } else {
            Log.e(TAG, "updateTestState=>There are no test items that can be updated.");
        }
    }

    /**
     * 获取测试组中的所有测试项
     *
     * @param context    　Context对象
     * @param list       　所有适用的测试项集合
     * @param groupResId 要获取的测试组的资源ID
     * @return 返回属于指定测试组的测试项集合
     */
    public static ArrayList<TestInfo> getGroupTestItems(Context context, ArrayList<TestInfo> list, int groupResId) {
        ArrayList<TestInfo> groupItems = new ArrayList<TestInfo>();
        if (list != null && list.size() > 0) {
            for (TestInfo info : list) {
                if (info.getGroupResId() == groupResId) {
                    groupItems.add(info);
                }
            }
            Log.i(TAG, "getGroupTestItems=>" + context.getString(groupResId) + " has " + groupItems.size() + " test items.");
        } else {
            Log.e(TAG, "getGroupTestItems=>no test items.");
        }
        return groupItems;
    }

    /**
     * 获取指定测试组的测试状态
     * @param context　Context对象
     * @param list　所有适用工厂测试的测试项集合
     * @param groupResId　测试组资源ID
     * @return 返回指定测试组的测试状态
     */
    public static TestState getGroupTestState(Context context, ArrayList<TestInfo> list, int groupResId) {
        TestState result = TestState.PASS;
        boolean isTest = false;
        FactoryTestDatabase ftd = FactoryTestDatabase.getInstance(context);
        if (list != null && list.size() > 0) {
            TestInfo item = null;
            TestState state = TestState.UNKNOWN;
            for (int i = 0; i < list.size(); i++) {
                item = list.get(i);
                if (item.getGroupResId() == groupResId) {
                    state = ftd.getTestState(item.getTestTitleResId());
                    if (TestState.FAIL.equals(state)) {
                        if (!isTest) {
                            isTest = true;
                        }
                        result = TestState.FAIL;
                        break;
                    } else if (TestState.PASS.equals(state)) {
                        if (!isTest) {
                            isTest = true;
                        }
                    } else {
                        result = TestState.UNKNOWN;
                    }
                }
            }
        }
        if (TestState.PASS.equals(result) && !isTest) {
            Log.i(TAG, "getGroupTestState=>" + context.getString(groupResId) + " is not test.");
            result = TestState.UNKNOWN;
        }
        Log.i(TAG, "getGroupTestState=>" + context.getString(groupResId) + " test state is " + result.name());
        return result;
    }

    /**
     * 工厂测试自动测试
     * @param context　Context对象
     * @param list　所有适用的测试项集合
     * @param index　当前要测试的测试项下标
     */
    public static void startAutoTest(Context context, ArrayList<TestInfo> list, int index) {
        // 开始测试时，首先清除上次测试结果
        if (index == 0) {
            FactoryTestDatabase.getInstance(context).clearDatabase();
        }
        if (list != null && list.size() > 0) {
            if (index < list.size()) {
                TestInfo info = list.get(index);
                Log.i(TAG, "startAutoTest=>start auto test " + context.getString(info.getTestTitleResId()));
                Intent intent = info.getIntent();
                intent.putExtra(EXTRA_TEST_INDEX, index);
                intent.putExtra(EXTRA_AUTO_TEST, true);
                context.startActivity(intent);
            } else {
                Log.e(TAG, "startAutoTest=>" + index + " is out of test list size " + list.size());
            }
        } else {
            Log.e(TAG, "startAutoTest=>no test item to test.");
        }
    }

    /**
     * 测试项排序比对方法
     * 首先通过比较测试项所在的测试集合的优先级。如果测试项所属的测试集合优先级相同，则比较该测试项在测试集合中的优先级。
     */
    private static Comparator<TestInfo> mTestInfoComparable = new Comparator<TestInfo>() {
        @Override
        public int compare(TestInfo item1, TestInfo item2) {
            if (item1.getGroupPriority() > item2.getGroupPriority()) {
                return 1;
            } else if (item1.getGroupPriority() < item2.getGroupPriority()) {
                return -1;
            } else {
                if (item1.getPriority() > item2.getPriority()) {
                    return 1;
                } else if (item1.getPriority() < item2.getPriority()) {
                    return -1;
                } else {
                    return 0;
                }
            }
        }
    };

}
