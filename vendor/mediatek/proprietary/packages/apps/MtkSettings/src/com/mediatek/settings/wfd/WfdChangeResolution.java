package com.mediatek.settings.wfd;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.display.DisplayManager;
import android.hardware.display.WifiDisplayStatus;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pManager;
import android.os.UserHandle;
import android.provider.Settings;
import androidx.preference.SwitchPreference;
import androidx.preference.PreferenceScreen;
import androidx.fragment.app.FragmentManager;

import android.text.TextUtils;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import com.android.settings.ProgressCategory;
import com.android.settings.R;
import com.mediatek.settings.FeatureOption;
import com.mediatek.provider.MtkSettingsExt;

import java.util.ArrayList;
import java.util.Arrays;

public class WfdChangeResolution {
    private static final String TAG = "WfdChangeResolution";
    private Context mContext;
    private DisplayManager mDisplayManager;

    // Change resolution menu id
    private static final int MENU_ID_CHANGE_RESOLUTION = Menu.FIRST + 1;

    /*
     * Device resolution:
     * 0: 720p 30fps menu disabled
     * 1: 1080p 30fps menu disabled
     * 2: 1080p 30fps
     * 3: 720p 30fps
     */
    public static final ArrayList<Integer> DEVICE_RESOLUTION_LIST = new ArrayList(
            Arrays.asList(2, 3));

    // WFD sink supported
    private SwitchPreference mDevicePref;
    private WifiP2pDevice mP2pDevice;
    // WFD sink UIBC supported
    private static final String FLOAT_MENU_PACKAGE = "com.mediatek.floatmenu";
    private static final String FLOAT_MENU_CLASS = "com.mediatek.floatmenu.FloatMenuService";

    public WfdChangeResolution(Context context) {
        mContext = context;
        mDisplayManager = (DisplayManager) mContext
                .getSystemService(Context.DISPLAY_SERVICE);
    }

    /**
     * Add change resolution option menu.
     *
     * @param menu
     *            the menu that change resolution menu item will be added
     * @param status
     *            current WFD status
     */
    public void onCreateOptionMenu(Menu menu, WifiDisplayStatus status) {
        int currentResolution = Settings.Global.getInt(mContext.getContentResolver(),
                MtkSettingsExt.Global.WIFI_DISPLAY_RESOLUTION, 0);
        Log.d("@M_" + TAG, "current resolution is " + currentResolution);
        if (DEVICE_RESOLUTION_LIST.contains(currentResolution)) {
            menu.add(Menu.NONE, MENU_ID_CHANGE_RESOLUTION,
                0, R.string.wfd_change_resolution_menu_title)

                    .setEnabled(
                        status.getFeatureState() == WifiDisplayStatus.FEATURE_STATE_ON
                        && status.getActiveDisplayState()
                        != WifiDisplayStatus.DISPLAY_STATE_CONNECTING)
                    .setShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        }
    }

    /**
     * Called when the option menu is selected.
     *
     * @param item
     *            the selected menu item
     * @param fragmentManager
     *            Fragment manager used to show new fragment
     * @return true, change resolution item is selected, otherwise false
     */
    public boolean onOptionMenuSelected(MenuItem item, FragmentManager fragmentManager) {
        if (item.getItemId() == MENU_ID_CHANGE_RESOLUTION) {
            new WfdChangeResolutionFragment().show(
                    fragmentManager, "change resolution");
            return true;
        }
        return false;
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.v("@M_" + TAG, "receive action: " + action);
            if (WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION
                    .equals(action)) {
                mP2pDevice = (WifiP2pDevice) intent
                        .getParcelableExtra(WifiP2pManager.EXTRA_WIFI_P2P_DEVICE);
                updateDeviceName();
            }
        }
    };

    /**
     * Add additional preference exception default preferences.
     *
     * @param preferenceScreen
     *            UI preference screen
     * @param available
     *            WFD is available
     * @return True if new preference added
     */
    public boolean addAdditionalPreference(PreferenceScreen preferenceScreen,
            boolean available) {
        boolean added = false;
        if (available && FeatureOption.MTK_WFD_SINK_SUPPORT) {
            if (mDevicePref == null) {
                mDevicePref = new SwitchPreference(mContext);
                if (mContext.getResources().
                    getBoolean(com.android.internal.R.bool.config_voice_capable)) {
                    mDevicePref.setIcon(R.drawable.ic_wfd_cellphone);    // for phones
                } else {
                    mDevicePref.setIcon(R.drawable.ic_wfd_laptop);   // for tablets, etc.
                }
                mDevicePref.setPersistent(false);
                mDevicePref.setSummary(R.string.wfd_sink_summary);
                mDevicePref.setOrder(2);
                Intent intent = new Intent("mediatek.settings.WFD_SINK_SETTINGS");
                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                mDevicePref.setIntent(intent);
            }
            preferenceScreen.addPreference(mDevicePref);
            updateDeviceName();
            ProgressCategory cat = new ProgressCategory(mContext, null, 0);
            cat.setEmptyTextRes(R.string.wifi_display_no_devices_found);
            cat.setOrder(3);
            cat.setTitle(R.string.wfd_device_category);
            preferenceScreen.addPreference(cat);
            added = true;
        }
        return added;
    }

    /**
     * Called when activity started.
     */
    public void onStart() {
        Log.d("@M_" + TAG, "onStart");
        if (FeatureOption.MTK_WFD_SINK_SUPPORT) {
            IntentFilter filter = new IntentFilter();
            filter
                    .addAction(WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION);
            mContext.registerReceiver(mReceiver, filter);
        }
    }

    /**
     * Called when activity stopped.
     */
    public void onStop() {
        Log.d("@M_" + TAG, "onStop");
        if (FeatureOption.MTK_WFD_SINK_SUPPORT) {
            mContext.unregisterReceiver(mReceiver);
        }
    }

    private void updateDeviceName() {
        if (mP2pDevice != null && mDevicePref != null) {
            if (TextUtils.isEmpty(mP2pDevice.deviceName)) {
                mDevicePref.setTitle(mP2pDevice.deviceAddress);
            } else {
                mDevicePref.setTitle(mP2pDevice.deviceName);
            }
        }
    }

    /**
     * Handle WFD status changed event.
     *
     * @param status
     *            The latest WFD status
     */
    public void handleWfdStatusChanged(WifiDisplayStatus status) {
        if (!FeatureOption.MTK_WFD_SINK_SUPPORT) {
            return;
        }
        boolean bStateOn = (status != null
            && status.getFeatureState() == WifiDisplayStatus.FEATURE_STATE_ON);
        Log.d("@M_" + TAG, "handleWfdStatusChanged bStateOn: " + bStateOn);
        if (bStateOn) {
            int wfdState = status.getActiveDisplayState();
            Log.d("@M_" + TAG, "handleWfdStatusChanged wfdState: " + wfdState);
            handleWfdStateChanged(wfdState, isSinkMode());
        } else {
            handleWfdStateChanged(
                    WifiDisplayStatus.DISPLAY_STATE_NOT_CONNECTED,
                    isSinkMode());
        }
    }

    private void handleWfdStateChanged(int wfdState, boolean sinkMode) {
        switch (wfdState) {
        case WifiDisplayStatus.DISPLAY_STATE_NOT_CONNECTED:
            if (!sinkMode) {
                if (mDevicePref != null) {
                    mDevicePref.setEnabled(true);
                    mDevicePref.setChecked(false);
                }
                if (FeatureOption.MTK_WFD_SINK_UIBC_SUPPORT) {
                    Intent intent = new Intent();
                    intent.setClassName(FLOAT_MENU_PACKAGE, FLOAT_MENU_CLASS);
                    mContext.stopServiceAsUser(intent, UserHandle.CURRENT);
                }
            }
            break;
        case WifiDisplayStatus.DISPLAY_STATE_CONNECTING:
            if (!sinkMode) {
                if (mDevicePref != null) {
                    mDevicePref.setEnabled(false);
                }
            }
            break;
        case WifiDisplayStatus.DISPLAY_STATE_CONNECTED:
            if (!sinkMode) {
                if (mDevicePref != null) {
                    mDevicePref.setEnabled(false);
                }
            }
            break;
        default:
            break;
        }
    }

    /**
     * Called when select one router to connect.
     */
    public void prepareWfdConnect() {
        if (FeatureOption.MTK_WFD_SINK_UIBC_SUPPORT) {
            Intent intent = new Intent();
            intent.setClassName(FLOAT_MENU_PACKAGE, FLOAT_MENU_CLASS);
            mContext.startServiceAsUser(intent, UserHandle.CURRENT);
        }
    }

    private boolean isSinkMode() {
        return mDisplayManager.isSinkEnabled();
    }

}
