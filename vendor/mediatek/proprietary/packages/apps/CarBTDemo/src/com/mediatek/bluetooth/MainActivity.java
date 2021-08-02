package com.mediatek.bluetooth;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import android.widget.EditText;
import android.bluetooth.BluetoothHeadsetClient;
import android.bluetooth.BluetoothHeadsetClientCall;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothA2dpSink;
import android.bluetooth.BluetoothAvrcpController;
import android.bluetooth.BluetoothAvrcp;
import android.bluetooth.BluetoothPbapClient;
import android.view.KeyEvent;
import android.os.Handler;
import android.os.Message;
//import com.mediatek.bluetooth.BluetoothProfileManager;
//import com.mediatek.bluetooth.BluetoothProfileManager.Profile;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientManager;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientConstants;
import com.mediatek.bluetooth.util.Utils;
import android.annotation.SuppressLint;
import android.app.ActionBar;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.util.DisplayMetrics;
import android.util.Log;
import android.text.TextUtils;
import android.view.Menu;
import android.view.Window;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import com.mediatek.bluetooth.common.*;
import android.widget.Toast;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.MotionEvent;
import android.view.inputmethod.InputMethodManager;
import java.util.Set;


/*@SuppressLint("NewApi")*/
public class MainActivity extends FragmentActivity implements ActionBar.TabListener {

    private static final String TAG = "MainActivity";

    private static final int DIAL_PAGE = 0;
    private static final int CALL_HISTORY_PAGE = 1;
    private static final int PHONEBOOK_PAGE = 2;
    private static final int MUSIC_PAGE = 3;
    private static final int SETTINGS_PAGE = 4;
    public static int screenW;
    public static int screenH;
    private long firstTime = 0;
    private static final String REQUEST_ROLE = "request_role";
    private static final String DEVICE_ADDRESS = "device_address_to_connect";
    private static final String CONNECT_OR_CONFIRM_ACTION = "action";

    private static final String REQUEST_ROLE_SOURCE = "source";
    private static final String REQUEST_ROLE_SINK = "sink";

    private static final int[] tab_icons = { R.drawable.bt_dialpad, R.drawable.bt_history,
            R.drawable.bt_phonebook, R.drawable.bt_music, R.drawable.bt_settings };

    /**
     * The {@link android.support.v4.view.PagerAdapter} that will provide
     * fragments for each of the sections. We use a
     * {@link android.support.v4.app.FragmentPagerAdapter} derivative, which
     * will keep every loaded fragment in memory. If this becomes too memory
     * intensive, it may be best to switch to a
     * {@link android.support.v4.app.FragmentStatePagerAdapter}.
     */
    SectionsPagerAdapter mSectionsPagerAdapter;
    List<Fragment> mFragmentList = new ArrayList<Fragment>();

    /**
     * The {@link ViewPager} that will host the section contents.
     */
    ViewPager mViewPager;

    DialFragment mDialFragment;
    CallHistoryFragment mCallHistoryFragment;
    PhoneBookFragment mPhonebookFragment;
    MusicFragment mMusicFragment;
    SettingsFragment mSettingsFragment;
    private AudioManager mAudiomanager;
    BluetoothHeadsetClient mHeadsetClient = null;
    private boolean IsHfClientConnected = false;
    public static BluetoothDevice mConnectedDevice = null;

    private LocalBluetoothAdapter mLocalAdapter;
    private LocalBluetoothManager mLocalManager;
    private BluetoothA2dpSink mService;

    private BluetoothAvrcpController mAVRCPControllerService;
    private boolean mIsProfileReady;
    private BluetoothPbapClientManager mPbapClientManager = null;

    Bundle mArgsCall = null;
    Bundle mArgsInfo = null;
    private final int MESSAGE_INIT_PROFY = 0;
    private final int RECHECK_PATH_DELAY = 150;
    private IntentFilter intentFilter;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch(msg.what){
                case MESSAGE_INIT_PROFY:
                    // get HFP Client Proxy from BT adapter
                    BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
                    if (adapter != null) {
                        adapter.getProfileProxy(MainActivity.this, mProfileServiceListener, BluetoothProfile.HEADSET_CLIENT);
                    }
                    break;
            }

        }

    };

    private void getScreenSize(){
        DisplayMetrics metric = new DisplayMetrics();
        this.getWindowManager().getDefaultDisplay().getMetrics(metric);
        screenH = metric.heightPixels;
        screenW = metric.widthPixels;

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        Log.d(TAG, "onCreate");
        if(savedInstanceState != null){
            String FRAGMENT_FLAGS = "android:support:fragments";
            savedInstanceState.remove(FRAGMENT_FLAGS);
        }

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getScreenSize();
        // Set up the action bar.
        final ActionBar actionBar = getActionBar();
        actionBar.setDisplayShowTitleEnabled(false);
        actionBar.setDisplayShowHomeEnabled(false);
        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);

        // create fragments
        initFragment();

        // Create the adapter that will return a fragment for each of the three
        // primary sections of the app.
        mSectionsPagerAdapter = new SectionsPagerAdapter(getSupportFragmentManager());

        // Set up the ViewPager with the sections adapter.
        mViewPager = (ViewPager) findViewById(R.id.pager);
        mViewPager.setAdapter(mSectionsPagerAdapter);

        // When swiping between different sections, select the corresponding
        // tab. We can also use ActionBar.Tab#select() to do this if we have
        // a reference to the Tab.
        mViewPager.setOnPageChangeListener(new ViewPager.SimpleOnPageChangeListener() {
            @Override
            public void onPageSelected(int position) {
                actionBar.setSelectedNavigationItem(position);
            }
        });

        // For each of the sections in the app, add a tab to the action bar.
        for (int i = 0; i < mSectionsPagerAdapter.getCount(); i++) {
            // Create a tab with text corresponding to the page title defined by
            // the adapter. Also specify this Activity object, which implements
            // the TabListener interface, as the callback (listener) for when
            // this tab is selected.
            actionBar.addTab(actionBar.newTab().setText(mSectionsPagerAdapter.getPageTitle(i))
                    .setIcon(tab_icons[i]).setTabListener(this));
        }

        registerForBroadcasts();
        mLocalManager = Utils.getLocalBtManager(this);
        if (mLocalManager == null) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
            finish();
        }

        mLocalAdapter = mLocalManager.getBluetoothAdapter();
        mAudiomanager = new AudioManager(this);
        mPbapClientManager = BluetoothPbapClientManager.getInstance();
        Message msg_init_profy = mHandler.obtainMessage(MESSAGE_INIT_PROFY,
                RECHECK_PATH_DELAY);
        mHandler.sendMessage(msg_init_profy);

    }

    private void initFragment() {
        mDialFragment = new DialFragment();
        mCallHistoryFragment = new CallHistoryFragment(this);
        mPhonebookFragment = new PhoneBookFragment(this);
        mMusicFragment = new MusicFragment();
        mSettingsFragment = new SettingsFragment(this);
        mFragmentList.add(mDialFragment);
        mFragmentList.add(mCallHistoryFragment);
        mFragmentList.add(mPhonebookFragment);
        mFragmentList.add(mMusicFragment);
        mFragmentList.add(mSettingsFragment);
    }

    protected void onDestroy() {
        Log.d(TAG, "onDestroy");
        this.unregisterReceiver(mReceiver);
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if (adapter != null) {
            adapter.closeProfileProxy(BluetoothProfile.HEADSET_CLIENT,
                (BluetoothProfile)mHeadsetClient);
        }
        super.onDestroy();

    }


    private final BluetoothProfile.ServiceListener mProfileServiceListener =
        new BluetoothProfile.ServiceListener() {
            public void onServiceConnected(int profile, BluetoothProfile proxy) {
                Log.d(TAG, "onServiceConnected :" + proxy);
                mHeadsetClient = (BluetoothHeadsetClient) proxy;
                mDialFragment.setHeadsetClientProxy(mHeadsetClient);
                if(mHeadsetClient.getConnectedDevices().size()>0){
                    mConnectedDevice = mHeadsetClient.getConnectedDevices().get(0);
                    IsHfClientConnected = true;
                    mDialFragment.setHeadsetClientDeviceState(mConnectedDevice,IsHfClientConnected);
                }

            }

            public void onServiceDisconnected(int profile) {
                Log.d(TAG, "mHeadsetClient Disconnected :");
                IsHfClientConnected = false;
                mConnectedDevice = null;
            }
        };

    @Override
    public void onTabSelected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
        // When the given tab is selected, switch to the corresponding page in
        // the ViewPager.
        mViewPager.setCurrentItem(tab.getPosition());

        if(CALL_HISTORY_PAGE == tab.getPosition()){
            mPhonebookFragment.unRegisterSyncCallBack();
        }
        else if(PHONEBOOK_PAGE == tab.getPosition()){
            mPhonebookFragment.registerSyncCallBack();
        }else {
            mPhonebookFragment.unRegisterSyncCallBack();
        }
    }

    @Override
    public void onTabUnselected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
    }

    @Override
    public void onTabReselected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
    }

    private void registerForBroadcasts() {
        intentFilter = new IntentFilter();
        //intentFilter.addAction(BluetoothProfileManager.ACTION_PROFILE_STATE_UPDATE);
        intentFilter.addAction(BluetoothDevice.ACTION_PAIRING_REQUEST);
        intentFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        intentFilter.addAction(BluetoothHeadsetClient.ACTION_CONNECTION_STATE_CHANGED);
        intentFilter.addAction(BluetoothPbapClient.ACTION_CONNECTION_STATE_CHANGED);
        intentFilter.addAction(DialFragment.ACTION_BLUETOOTH_CALLOUT);
        this.registerReceiver(mReceiver, intentFilter);
    }

    /**
     * A {@link FragmentPagerAdapter} that returns a fragment corresponding to
     * one of the sections/tabs/pages.
     */
    public class SectionsPagerAdapter extends FragmentPagerAdapter {

        public SectionsPagerAdapter(android.support.v4.app.FragmentManager fragmentManager) {
            super(fragmentManager);
        }

        @Override
        public Fragment getItem(int position) {
            return mFragmentList.get(position);
        }

        @Override
        public int getCount() {
            // Show 2 total pages.
            return mFragmentList.size();
        }

        @Override
        public CharSequence getPageTitle(int position) {
            Locale l = Locale.getDefault();
            switch (position) {
            case DIAL_PAGE:
                return getString(R.string.tab_dial);
            case CALL_HISTORY_PAGE:
                return getString(R.string.tab_call_history);
            case PHONEBOOK_PAGE:
                return getString(R.string.tab_phone_book);
            case MUSIC_PAGE:
                return getString(R.string.tab_music);
            case SETTINGS_PAGE:
                return getString(R.string.tab_settings);
            }
            return null;
        }

    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            //// TODO: put this in callback instead of receiving
            Log.d(TAG, "onReceive:action->" + intent.getAction());

            /*if (intent.getAction().equals(BluetoothProfileManager.ACTION_PROFILE_STATE_UPDATE)) {
                Profile profilename = (Profile) intent
                        .getSerializableExtra(BluetoothProfileManager.EXTRA_PROFILE);
                int profilestate = intent.getIntExtra(BluetoothProfileManager.EXTRA_NEW_STATE,
                        BluetoothProfileManager.STATE_DISCONNECTED);
                if (profilename.equals(BluetoothProfileManager.Profile.A2DP)
                        || profilename.equals(BluetoothProfileManager.Profile.AVRCP)) {
                        Log.d(TAG,"profilename+++");
                    ((MusicFragment) mMusicFragment).updateState(profilename, profilestate);
                }
            }else */if(intent.getAction().equals(
                BluetoothHeadsetClient.ACTION_CONNECTION_STATE_CHANGED)) {

                int new_state = intent.getIntExtra(
                    BluetoothProfile.EXTRA_STATE,
                    BluetoothProfile.STATE_DISCONNECTED);
                int prev_state = intent.getIntExtra(
                    BluetoothProfile.EXTRA_PREVIOUS_STATE,
                    BluetoothProfile.STATE_DISCONNECTED);
                mConnectedDevice = (BluetoothDevice)intent.getParcelableExtra(
                    BluetoothDevice.EXTRA_DEVICE);
                if(new_state != BluetoothProfile.STATE_CONNECTED){
                   IsHfClientConnected = false;
                   mCallHistoryFragment.cleanData();
                   mPhonebookFragment.cleanData();
                }else{
                   IsHfClientConnected = true;
                }
                mDialFragment.setHeadsetClientDeviceState(mConnectedDevice,IsHfClientConnected);
            } else if (intent.getAction().equals(
                BluetoothPbapClient.ACTION_CONNECTION_STATE_CHANGED)){
                mConnectedDevice = (BluetoothDevice)intent.getParcelableExtra(
                    BluetoothDevice.EXTRA_DEVICE);

            }else if (intent.getAction().equals(DialFragment.ACTION_BLUETOOTH_CALLOUT)) {
                String calloutNumber = intent.getStringExtra(
                        DialFragment.EXTRA_BLUETOOTH_CALLOUT_NUMBER);
                String calloutName = intent.getStringExtra(
                        DialFragment.EXTRA_BLUETOOTH_CALLOUT_NAME);

                //add this code for the extension number
                calloutNumber = calloutNumber.replace(",", "p");

                Log.i(TAG, "ACTION_BLUETOOTH_CALLOUT " + calloutNumber);
                if (mHeadsetClient != null &&
                    mHeadsetClient.dial(mConnectedDevice,calloutNumber) != null) {
                }else {
                    Utils.showShortToast(MainActivity.this,R.string.str_bluetooth_phone_not_connected);

                }
            }

            }
    };


    @Override
    public boolean dispatchTouchEvent(MotionEvent ev) {
        if (ev.getAction() == MotionEvent.ACTION_DOWN) {
            View v = this.getCurrentFocus();
            if (isShouldHideInput(v, ev)) {
                Log.d(TAG,"hideInput");
                InputMethodManager imm = (InputMethodManager)this.getSystemService(Context.INPUT_METHOD_SERVICE);
                if (imm != null) {
                    imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
                }
            }
            return super.dispatchTouchEvent(ev);
        }

        if (this.getWindow().superDispatchTouchEvent(ev)) {
            return true;
        }
        return this.onTouchEvent(ev);
    }


    public boolean isShouldHideInput(View v, MotionEvent event) {
        if (v != null && (v instanceof EditText)) {
            int[] leftTop = { 0, 0 };

            v.getLocationInWindow(leftTop);
            int left = leftTop[0];
            int top = leftTop[1];
            int bottom = top + v.getHeight();
            int right = left + v.getWidth();
            if (event.getX() > left && event.getX() < right && event.getY() > top && event.getY() < bottom) {
                return false;
            } else {
                return true;
            }
        }
        return false;
    }
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        switch(keyCode){

        case KeyEvent.KEYCODE_BACK:
             long secondTime = System.currentTimeMillis();
              if (secondTime - firstTime > 2000) {
                  Utils.showShortToast(this,R.string.press_back_twice_to_exit);
                  firstTime = secondTime;
                  return true;
              } else {
                    BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
                    Set<BluetoothDevice> bondedDevices = adapter.getBondedDevices();
                    for(BluetoothDevice device : bondedDevices){
                       CachedBluetoothDevice cachedDevice =
                        mLocalManager.getCachedDeviceManager().findDevice(device);
                       if(cachedDevice != null)
                         Utils.disconnectBT(MainActivity.this,cachedDevice);
                    }

                  System.exit(0);
              }
            break;
        }
      return super.onKeyUp(keyCode, event);

    }

}
