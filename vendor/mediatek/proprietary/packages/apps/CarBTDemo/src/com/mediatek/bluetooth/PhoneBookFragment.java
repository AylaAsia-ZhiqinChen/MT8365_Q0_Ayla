package com.mediatek.bluetooth;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.v4.app.Fragment;
import android.support.v4.app.LoaderManager;
import android.support.v4.content.CursorLoader;
import android.support.v4.content.Loader;
import android.database.Cursor;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.view.Menu;
import android.view.View.OnFocusChangeListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.SimpleAdapter;
import android.widget.TabHost;
import android.widget.TabHost.OnTabChangeListener;
import android.widget.TabWidget;
import android.view.Window;
import android.view.WindowManager;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;

import com.mediatek.bluetooth.util.ActionItem;
import com.mediatek.bluetooth.util.QuickActions;
import com.mediatek.bluetooth.R;
import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothAdapter;
//import com.mediatek.bluetooth.BluetoothProfileManager;
//import com.mediatek.bluetooth.BluetoothProfileManager.Profile;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.BaseAdapter;
import android.view.ViewGroup;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.ProgressBar;
import android.widget.SimpleAdapter;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;
import android.bluetooth.BluetoothHeadsetClient;
import android.bluetooth.BluetoothHeadsetClientCall;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothPbapClient;
import com.android.vcard.VCardEntry;
import com.android.vcard.VCardEntry.PhoneData;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientManager;
import com.mediatek.bluetooth.pbapclient.BluetoothPabapClientCallback;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientConstants;
import com.mediatek.bluetooth.util.Utils;
import android.text.InputType;
import android.content.DialogInterface;
import android.app.AlertDialog;

import android.util.Log;

public class PhoneBookFragment extends Fragment{

    protected static final String TAG = "PhoneBookFragment";

    Context mContext;

    private static final boolean DEBUG=false;

    private static final int ITEM_COUNT_PER_PAGE = 20;
    private static final int MESSAGE_GET_SEARCH_PHONEBOOK = 0;
    private static final int MESSAGE_PULL_PHONEBOOK_SIZE = 1;
    private static final int MESSAGE_GET_PHONEBOOK = 2;
    private static final int MESSAGE_SET_PATH_FAIL = 3;
    private static final int MAX_FAILED_TIMES = 5;
    private static final int MESSAGE_TOAST_PULL_PHONEBOOK_SIZE_FAIL = 2;
    private static final int MESSAGE_CLEAR_DATA = 6;
    private static final int MESSAGE_DISMISS_POP = 7;

    private static final int GET_PHONEBOOK_DELAY = 5000;
    private static final int GET_SEARCH_PHONEBOOK_DELAY = 1000;

    private static final int RECHECK_PATH_DELAY = 200;
    private static final int MAX_SET_PATH_TIMES = 10;
    private static final int MAX_GET_PHONEBOOK_SIZE = 3;

    public static final int ACTIVITY_RESULT_CODE = 100;
    private static final int MAX_LIST_COUNT = 65535;
    private static final int MODE_INPUT_NAME = 0;//BluetoothPbapClient.SEARCH_ATTR_NAME;
    private static final int MODE_INPUT_NUMBER = 1;//BluetoothPbapClient.SEARCH_ATTR_NUMBER;

    public static final String EXTRA_BT_ADDRESS = "address";
    private byte mCurrentMode = MODE_INPUT_NAME;

    private BluetoothPbapClientManager mManager = BluetoothPbapClientManager.getInstance();

    private int mCurrentPage = 0;
    private int mGetPhonebookSizeTimes = 0;
    private int fail_times = 0;

    private BluetoothPbapClient mService;
    public static BluetoothDevice mConnectedDevice = null;
    public static int mPbapClientProfileState = BluetoothProfile.STATE_DISCONNECTED;

    private ArrayList<ContactData> mContactsList = new ArrayList<ContactData>();
    private ArrayList<ContactData> mPbapSearchList = new ArrayList<ContactData>();
    private ContactsAdapter mAdapter = new ContactsAdapter();
    private SearchedVcardAdapter mSearchAdapter = new SearchedVcardAdapter();

    private static final int CONTACT_LOADER_ID = 1;
    private static final int SEARCH_LOADER_ID = 2;

    private static final String[] Contact_PROJECTION = {
        ContactsContract.CommonDataKinds.Phone.DISPLAY_NAME,
        ContactsContract.CommonDataKinds.Phone.NUMBER
    };

    private Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
        Log.d(TAG, "handleMessage msg.what->" + msg.what);
            switch (msg.what) {
                case MESSAGE_GET_SEARCH_PHONEBOOK:
                    getSearchPhonebook();
                    break;
                case MESSAGE_PULL_PHONEBOOK_SIZE:
                    mGetPhonebookSizeTimes++;
                    if (mGetPhonebookSizeTimes > MAX_GET_PHONEBOOK_SIZE) {
                        Utils.dismissPopupWindow();
                    } else {
                        if(!pullPhoneBookSize()){
                            Utils.dismissPopupWindow();
                        }
                    }
                    break;
                case MESSAGE_GET_PHONEBOOK:
                    getPhonebook();
                    break;
                case MESSAGE_SET_PATH_FAIL:
                    fail_times++;
                    if(fail_times<MAX_FAILED_TIMES){
                        //checkFolderPath_forsearch();
                    }else{
                        Utils.dismissPopupWindow();
                        Utils.showShortToast(getActivity(),"set path fail");
                    }
                    break;
                case MESSAGE_CLEAR_DATA:
                    Log.d(TAG,"start clear");
                    mContactsList.clear();
                    mPbapSearchList.clear();
                    mAdapter.notifyDataSetChanged();
                    mSearchAdapter.notifyDataSetChanged();
                    break;
                case MESSAGE_DISMISS_POP:
                    Utils.dismissPopupWindow();
                    break;

            }

        };
    };
    private Button search_btn;
    private Button mSwitchMode;
    private Button pb_btn;
    private Button clear_btn;
    private EditText mSearchValue;
    private PhonebookButtonOnclick mOnClickLietener = new PhonebookButtonOnclick();

    private ListView mPhonebookListView;
    private ListView mSearchResultView;

    //private SimpleAdapter mAdapter;  //Replace ListAdapter
    private int mDownloadPath;
    private int mShowedRecordNum;

    private long mStartTime;
    private long mEndTime;

    /*
    * for change tab
    */
    private boolean mIsSyncing = false;

    /*
    * Sync State, for recieve call intent
    */
    private boolean mSyncState = false;

    /*
    * Stop Sync Active or Passive
    */
    private boolean mStopSyncActive = false;

    /*
    * last time download bt addr
    */
    private String mLastDwnldAddr = "";

    /*
    * is Sync finish
    */
    private boolean mIsSyncFinish = false;

    /*
    * is loading data
    */
    private boolean mIsLoadingData = false;	
    private ActionItem action_call = new ActionItem();


    public PhoneBookFragment() {}


    public PhoneBookFragment(Context context) {
        mContext = context;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if (adapter != null) {
            adapter.getProfileProxy(getActivity(), PbapClientServiceListener,
                    BluetoothProfile.PBAP_CLIENT);
        }
    }
    private final BluetoothProfile.ServiceListener PbapClientServiceListener =
            new BluetoothProfile.ServiceListener() {

        public void onServiceConnected(int profile, BluetoothProfile proxy) {
            Log.d(TAG, "PbapClientServiceListener,proxy is : " + proxy.toString());
            mService = (BluetoothPbapClient) proxy;
            Log.d(TAG, "PbapClientServiceListener,device size : " + mService.getConnectedDevices().size());
            if(mService.getConnectedDevices().size()>0){
                mConnectedDevice = mService.getConnectedDevices().get(0);
                mPbapClientProfileState = BluetoothProfile.STATE_CONNECTED;
                Log.d(TAG, "onServiceConnected, mPbapClientProfileState = " + mPbapClientProfileState);
            }

        }

        public void onServiceDisconnected(int profile) {
            Log.d(TAG, "PbapClientServiceListener disconnected profile is : " + profile);
            mConnectedDevice = null;
            mPbapClientProfileState = BluetoothProfile.STATE_DISCONNECTED;
        }
    };

    @Override
    public void onResume() {
        Log.d(TAG,"onResume");
        mSearchValue.setFocusable(true);
        if(this.getUserVisibleHint()){
            registerSyncCallBack();
            Log.d(TAG,"onResume registercallback");
        }
        if(mPbapClientProfileState == BluetoothProfile.STATE_DISCONNECTED){
            Log.d(TAG,"BluetoothPbapClient not connected");
            cleanData();
        }
        Utils.dismissPopupWindow();
        registerReceiver();
        super.onResume();
    }


    @Override
    public void onStop() {
        Utils.dismissPopupWindow();
        super.onStop();
        clearLoader();
    }

    @Override
    public void onPause() {
        super.onPause();
    }
    

    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy unRegisterSyncCallBack");
        unRegisterSyncCallBack();
        try{
           if(mReceiver != null) {
               this.getActivity().unregisterReceiver(mReceiver);
            }
        }catch(IllegalArgumentException e){
           Log.e("TAG","IllegalArgumentException");
        }

        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if (adapter != null) {
            adapter.closeProfileProxy(BluetoothProfile.PBAP_CLIENT,
                (BluetoothProfile)mService);
        }
        Utils.dismissPopupWindow();
        super.onDestroy();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.bt_phonebook, container, false);

        pb_btn = (Button)view.findViewById(R.id.bt_phonebook_pb);
        clear_btn = (Button)view.findViewById(R.id.bt_phonebook_clear);
        search_btn = (Button)view.findViewById(R.id.bt_phonebook_search);
        mSearchValue = (EditText)view.findViewById(R.id.bt_phonebook_edit);
        mSwitchMode = (Button)view.findViewById(R.id.bt_phonebook_search_mode);

        clear_btn.setOnClickListener(mOnClickLietener);
        pb_btn.setOnClickListener(mOnClickLietener);
        search_btn.setOnClickListener(mOnClickLietener);
        mSwitchMode.setOnClickListener(mOnClickLietener);
        mSearchValue.setOnClickListener(mOnClickLietener);

        mPhonebookListView = (ListView)view.findViewById(R.id.phonebook_listview);
        mPhonebookListView.setAdapter(mAdapter);
        mPhonebookListView.setOnItemClickListener(mPhoneBookListClickListener);
        mAdapter.notifyDataSetChanged();

        mSearchResultView = (ListView)view.findViewById(R.id.phonebook_result_listview);
        mSearchResultView.setAdapter(mSearchAdapter);
        mSearchResultView.setOnItemClickListener(msearchResultListClickListener);
        mSearchAdapter.notifyDataSetChanged();

        updateSwitchModeText();
        return view;
    }

    public void registerSyncCallBack(){
    }

    public void unRegisterSyncCallBack(){
    }

    private void clearLoader() {
        getLoaderManager().destroyLoader(CONTACT_LOADER_ID);
    }
    private void getSearchPhonebook() {
        getLoaderManager().destroyLoader(SEARCH_LOADER_ID);
        Log.d(TAG, "getSearchPhonebook start");
        // Fetch the phone number from the contacts db using another loader.
        getLoaderManager().initLoader(SEARCH_LOADER_ID, Bundle.EMPTY,
            new LoaderManager.LoaderCallbacks<Cursor>() {
                @Override
                public Loader<Cursor> onCreateLoader(int id, Bundle args) {
                    return new CursorLoader(getContext(),
                        ContactsContract.CommonDataKinds.Phone.CONTENT_URI, Contact_PROJECTION,
                        null, null, null);
                }

                public void onLoadFinished(Loader<Cursor> loader, Cursor cursor) {
                    mPbapSearchList.clear();
                    while (cursor.moveToNext()) {
                        int nameColIdx = cursor.getColumnIndex(
                            ContactsContract.CommonDataKinds.Phone.DISPLAY_NAME);
                        String displayName = cursor.getString(nameColIdx);
                        int numberColIdx = cursor.getColumnIndex(
                            ContactsContract.CommonDataKinds.Phone.NUMBER);
                        String numbers = cursor.getString(numberColIdx);
                        //Log.d(TAG, "onLoadFinished, displayName = " + displayName);
                        Log.d(TAG, "onLoadFinished, numbers = " + numbers);
                        numbers = numbers.replace("-", "");
                        //Log.d(TAG, "onLoadFinished, numbers = " + numbers);
                        ContactData contactData = new ContactData();
                        if ((mCurrentMode == MODE_INPUT_NAME)
                            && (displayName.contains(mSearchValue.getText().toString()))){
                            contactData.setNumbers(numbers);
                            contactData.setDisplayName(displayName);
                            mPbapSearchList.add(contactData);
                        } else if ((mCurrentMode == MODE_INPUT_NUMBER)
                            && (numbers.contains(mSearchValue.getText().toString()))){
                            contactData.setNumbers(numbers);
                            contactData.setDisplayName(displayName);
                            mPbapSearchList.add(contactData);
                        }
                    }
                    mSearchAdapter.notifyDataSetChanged();
                    Log.d(TAG, "onLoadFinished, search list size = " + mPbapSearchList.size());

                    Message msg = mHandler.obtainMessage(MESSAGE_DISMISS_POP);
                    mHandler.sendMessage(msg);
                }

                public void onLoaderReset(Loader loader) {}
            });
    }

    private void getPhonebook(){
        clearLoader();
        Log.d(TAG, "getPhonebook start");
        // Fetch the phone number from the contacts db using another loader.
        getLoaderManager().initLoader(CONTACT_LOADER_ID, Bundle.EMPTY,
            new LoaderManager.LoaderCallbacks<Cursor>() {
                @Override
                public Loader<Cursor> onCreateLoader(int id, Bundle args) {
                    return new CursorLoader(getContext(),
                        ContactsContract.CommonDataKinds.Phone.CONTENT_URI, Contact_PROJECTION,
                        null, null, null);
                }

                public void onLoadFinished(Loader<Cursor> loader, Cursor cursor) {
                    mContactsList.clear();
                    while (cursor.moveToNext()) {
                        int nameColIdx = cursor.getColumnIndex(
                            ContactsContract.CommonDataKinds.Phone.DISPLAY_NAME);
                        String displayName = cursor.getString(nameColIdx);
                        int numberColIdx = cursor.getColumnIndex(
                            ContactsContract.CommonDataKinds.Phone.NUMBER);
                        String numbers = cursor.getString(numberColIdx);
                        Log.d(TAG, "onLoadFinished, numbers = " + numbers);
                        numbers = numbers.replace("-", "");
                        //Log.d(TAG, "onLoadFinished, numbers = " + numbers);

                        String sortKey = Utils.getPinYin(displayName).toLowerCase();
                        //Log.d(TAG, "onLoadFinished, sortKey = " + sortKey);

                        ContactData contactData = new ContactData();
                        contactData.setNumbers(numbers);
                        contactData.setDisplayName(displayName);
                        contactData.setSortKey(sortKey);
                        mContactsList.add(contactData);
                    }
                    Collections.sort(mContactsList);
                    mAdapter.notifyDataSetChanged();
                    Log.d(TAG, "onLoadFinished, contact list size = " + mContactsList.size());
                    Message msg = mHandler.obtainMessage(MESSAGE_DISMISS_POP);
                    mHandler.sendMessage(msg);
                }

                public void onLoaderReset(Loader loader) {}
            });
    }

    private class PhonebookButtonOnclick implements OnClickListener{

        //private int position;

        public PhonebookButtonOnclick() {
            //this.position = position;
        }

        @Override
        public void onClick(View v) {
            Log.d(TAG, "onClick, mPbapClientProfileState = " + mPbapClientProfileState);
            if(mPbapClientProfileState == BluetoothProfile.STATE_DISCONNECTED){
                Log.d(TAG, "onClick, MainActivity.mConnectedDevice = " + MainActivity.mConnectedDevice);
                Log.d(TAG, "onClick, mConnectedDevice = " + mConnectedDevice);
                if(MainActivity.mConnectedDevice != null){
                    Utils.showPbapConnectDialog(getActivity(),MainActivity.mConnectedDevice);
                }else{
                    Log.d(TAG,"PhonebookButtonOnclick device is null");
                    Utils.showShortToast(getActivity(),R.string.pbap_no_device_to_connect);
                }
                cleanData();
                return;
            }else if(mPbapClientProfileState == BluetoothProfile.STATE_CONNECTING){
                Utils.showShortToast(getActivity(),R.string.pbap_is_connecting);
                return;
            }

            // Button Click Listener
            switch (v.getId()) {
                case R.id.bt_phonebook_pb:
                    cleanData();
                    mPhonebookListView.setVisibility(View.VISIBLE);
                    mSearchResultView.setVisibility(View.GONE);
                    Utils.showPopupWindow(getActivity(),PhoneBookFragment.this.getView(),
                        MainActivity.screenW,MainActivity.screenH);
                    Message msg_pb = mHandler.obtainMessage(MESSAGE_GET_PHONEBOOK);
                    mHandler.sendMessageDelayed(msg_pb, GET_PHONEBOOK_DELAY);
                    break;
                case R.id.bt_phonebook_clear:
                    Log.d(TAG,"clear");
                    mContactsList.clear();
                    mPbapSearchList.clear();
                    mAdapter.notifyDataSetChanged();
                    mSearchAdapter.notifyDataSetChanged();
                    Log.d(TAG, "MainActivity.mConnectedDevice = " + MainActivity.mConnectedDevice);
                    if(MainActivity.mConnectedDevice != null){
                        Utils.disConnectPBAPClient(getActivity(),MainActivity.mConnectedDevice);
                        Utils.showShortToast(getActivity(),R.string.pbap_is_disconnected);
                    }
                    Utils.dismissPopupWindow();
                    break;
                case R.id.bt_phonebook_search:
                    mContactsList.clear();
                    mSearchAdapter.notifyDataSetChanged();
                    Utils.showPopupWindow(getActivity(),PhoneBookFragment.this.getView(),
                            MainActivity.screenW,MainActivity.screenH);
                    refreshSearchPhonebookList();
                    break;
                case R.id.bt_phonebook_search_mode:
                    if (mCurrentMode == MODE_INPUT_NAME) {
                        Utils.showShortToast(getActivity(),"Switch query mode to NUNBER");
                        mCurrentMode = MODE_INPUT_NUMBER;
                    } else {
                        Utils.showShortToast(getActivity(),"Switch query mode to NAME");
                        mCurrentMode = MODE_INPUT_NAME;
                    }
                    updateSwitchModeText();
                    break;
                case R.id.bt_phonebook_edit:
                    mSearchValue.setFocusable(true);
                    break;
                default:
                    break;
            }
        }
    }

    private class ContactData implements Comparable<ContactData>{
        private String mNumbers;
        private String mDisplayName;
        private String mSortKey;

        public String getNumbers(){
            return mNumbers;
        }
        public String getDisplayName(){
            return mDisplayName;
        }

        public void setNumbers(String numbers){
            mNumbers = numbers;
        }

        public void setDisplayName(String displayName){
            mDisplayName = displayName;
        }

        public String getSortKey(){
            return mSortKey;
        }
        public void setSortKey(String sortKey){
            mSortKey = sortKey;
        }
        @Override
        public int compareTo(@NonNull ContactData other) {
            return getSortKey().compareTo(other.getSortKey());
        }

    }
    /**
    * default phonebook
    *
    */
    private class ContactsAdapter extends BaseAdapter {

        @Override
        public int getCount() {
            if (mContactsList != null) {
                return mContactsList.size();
            } else {
                return 0;
            }
        }

        @Override
        public Object getItem(int position) {
            if (mContactsList != null) {
                return mContactsList.get(position);
            } else {
                return null;
            }
        }

        @Override
        public long getItemId(int position) {
            if (mContactsList != null) {
                return position ;
            } else {
                return 0;
            }
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            synchronized (mContactsList) {
                if (mContactsList != null && position < mContactsList.size()) {
                    ItemView view = null;
                    if (convertView != null && convertView instanceof ItemView) {
                        view = (ItemView) convertView;
                    } else {
                        view = new ItemView(mContext);
                    }
                    String displayName = mContactsList.get(position).getDisplayName();
                    if(DEBUG) Log.d(TAG,"name = " + displayName);
                    String numbers = mContactsList.get(position).getNumbers();
                    if(DEBUG) Log.d(TAG,"numbers = " + numbers);
                    view.getNameView().setText(displayName);
                    view.getNumberView().setText(numbers);
                    return view;
                } else {
                    return null;
                }
            }
        }

    }


    /*
    * for search result
    */
    private class SearchedVcardAdapter extends BaseAdapter {

        @Override
        public int getCount() {
            // TODO Auto-generated method stub
            return mPbapSearchList.size();
        }

        @Override
        public Object getItem(int position) {
            // TODO Auto-generated method stub
            return mPbapSearchList.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            synchronized (mPbapSearchList) {
                if (position < mPbapSearchList.size()) {
                    ItemView view = null;
                    if (convertView != null && convertView instanceof ItemView) {
                        view = (ItemView) convertView;
                    } else {
                        view = new ItemView(mContext);
                    }
                    String displayName = mPbapSearchList.get(position).getDisplayName();
                    String numbers = mPbapSearchList.get(position).getNumbers();
                    view.getNameView().setText(displayName);
                    view.getNumberView().setText(numbers);

                    return view;
                } else {
                    return null;
                }
            }
        }

    }

    private synchronized void checkFolderPath() {
        String currentFolder = mManager.getCurrentPath();
        Log.d(TAG, "checkFolderPath() started, mCurrentFolder = " + currentFolder);

        Log.d(TAG, "mCurrentFolder = " + currentFolder);
        if (!mManager.setPhoneBookFolderRoot()) {
            if(this.getUserVisibleHint()){
                Log.e(TAG, "setPhoneBookFolderRoot fail, reset it");
                Message message = mHandler.obtainMessage(MESSAGE_GET_PHONEBOOK);
                mHandler.sendMessageDelayed(message, GET_PHONEBOOK_DELAY);
            }
        }
    }

    private void pullPhoneBook(String pbName,int size) {
        Log.d(TAG, "pullPhoneBook pbName : " + pbName);
        if (mManager.pullPhoneBook(pbName, size, 0)) {
            Log.d(TAG, "pullPhoneBook start");
        } else {
            Utils.dismissPopupWindow();
            Utils.showShortToast(this.getActivity(),"pull phonebook error");
            Log.d(TAG, "pullPhoneBook error");
        }
    }

    private boolean pullPhoneBookSize() {
        return false;
    }

    private boolean pullVcardListingSize() {

        return false;
    }

    private void pullVcardList(String pbName, int currentPage) {

        String searchVal = mSearchValue.getText().toString();
        if (searchVal.isEmpty()) {
            searchVal = null;
        }
        byte order = 0;//BluetoothPbapClient.ORDER_BY_ALPHABETICAL;
        Log.d(TAG,"order = "+order+" mCurrentMode="+mCurrentMode+" searchVal="+searchVal);
        if (mManager.pullVcardList(pbName, order, mCurrentMode, searchVal, MAX_LIST_COUNT, 0)) {
            Log.d(TAG, "pullVcardList start");

        } else {
            Log.d(TAG, "pullVcardList error");
            /* Toast.makeText(getApplicationContext(), R.string.pull_failed,
            Toast.LENGTH_SHORT).show();*/
        }

    }

    private void updateSwitchModeText() {
        if (mCurrentMode == MODE_INPUT_NAME) {
            mSwitchMode.setText(R.string.bt_phonebook_search_mode_name);
            mSearchValue.setHint(getResources().getString(
                    R.string.bt_phonebook_search_hint_name));
            mSearchValue.setInputType(InputType.TYPE_CLASS_TEXT);
        } else {
            mSwitchMode.setText(R.string.bt_phonebook_search_mode_number);
            mSearchValue.setHint(getResources().getString(
                    R.string.bt_phonebook_search_hint_number));
            mSearchValue.setInputType(InputType.TYPE_CLASS_NUMBER);
        }
        mSearchValue.setText("");
    }

    // The on-click listener for all devices in the ListViews
    private OnItemClickListener mPhoneBookListClickListener = new OnItemClickListener() {
        public void onItemClick(AdapterView<?> av, View v, int arg2, long arg3) {

            final String pbname;
            final String pbnumber;
            Log.d(TAG,"mPhoneBookListClickListener, onItemClick");

            pbname = mContactsList.get(arg2).getDisplayName();
            pbnumber = mContactsList.get(arg2).getNumbers();
            if(pbnumber.isEmpty())
                return ;

            if(mPbapClientProfileState == BluetoothProfile.STATE_CONNECTED){
                Intent intent = new Intent();
                intent.setClass(getActivity(), VcardEntryActivity.class);
                intent.putExtra(VcardEntryActivity.DISPLAY_NAME, pbname);
                intent.putExtra(VcardEntryActivity.PHONE_NUMBER, pbnumber);
                Log.d(TAG,"start VcardEntryActivity with name:" + pbname + " pbnumber:" + pbnumber);
                startActivity(intent);
            }else{
                Utils.showShortToast(getActivity(),"Pbap is disconnected");
            }
        }
    };


    // The on-click listener for all devices in the ListViews
    private OnItemClickListener msearchResultListClickListener =
        new OnItemClickListener() {
            public void onItemClick(AdapterView<?> av, View v, int position, long arg3) {

                final String pbname;
                final String pbnumber;
                Log.d(TAG,"msearchResultListClickListener, onItemClick");

                pbname = mPbapSearchList.get(position).getDisplayName();
                pbnumber = mPbapSearchList.get(position).getNumbers();
                if(pbnumber.isEmpty())
                    return ;

                if(mPbapClientProfileState == BluetoothProfile.STATE_CONNECTED){
                    Intent intent = new Intent();
                    intent.setClass(getActivity(), VcardEntryActivity.class);
                    intent.putExtra(VcardEntryActivity.DISPLAY_NAME, pbname);
                    intent.putExtra(VcardEntryActivity.PHONE_NUMBER, pbnumber);
                    Log.d(TAG,"start VcardEntryActivity with name:" + pbname + " pbnumber:" + pbnumber);
                    startActivity(intent);
                }else{
                    Utils.showShortToast(getActivity(),"Pbap is disconnected");
                }
            }
        };


    private class ItemView extends LinearLayout {

        private TextView mNameView = null;
        private TextView mNumberView = null;
        public ItemView(Context context) {
            this(context, null);
        }

        public ItemView(Context context, AttributeSet attrs) {
            super(context, attrs);
            LayoutInflater inflator = (LayoutInflater) context
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            View view = inflator.inflate(R.layout.phonebook_listitem, null);
            addView(view);
            initView(view);
        }

        private void initView(View view) {
            mNameView = (TextView) findViewById(R.id.item_phonebook_name);
            mNumberView = (TextView) findViewById(R.id.item_phonebook_number);
        }

        public TextView getNameView() {
            return mNameView;
        }

        public TextView getNumberView() {
            return mNumberView;
        }
    }

    private void refreshSearchPhonebookList(){
        Log.d(TAG,"refreshSearchPhonebookList...");
        mSearchResultView.setVisibility(View.VISIBLE);
        mPhonebookListView.setVisibility(View.GONE);

        Message message0 = mHandler.obtainMessage(MESSAGE_GET_SEARCH_PHONEBOOK,
            GET_SEARCH_PHONEBOOK_DELAY);
        mHandler.sendMessage(message0);
    }

    // refresh the vcardlist after incoming call end
    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.v(TAG, "Received " + intent.getAction());

            String action = intent.getAction();
            if(action.equals(PhoneCallActivity.DESTORY_MSG)){
                if(getUserVisibleHint() && mSearchResultView.getVisibility() == View.VISIBLE){
                    refreshSearchPhonebookList();
                }
            }else if(action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)){
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                int bondState = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, BluetoothDevice.ERROR);
                if(bondState == BluetoothDevice.BOND_NONE){
                    if(mManager == null || mManager.getDevice()== null)return;
                    if(device.getAddress().equals(mManager.getDevice().getAddress())){
                        cleanData();
                    }
                }

            }else if(action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)){
                int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,BluetoothAdapter.STATE_OFF);
                if(state == BluetoothAdapter.STATE_OFF || state == BluetoothAdapter.STATE_TURNING_OFF){
                    Utils.dismissPbapConnectDialog();
                    if(mManager == null)return;
                    cleanData();
                }
            } else if (action.equals(BluetoothPbapClient.ACTION_CONNECTION_STATE_CHANGED)){
                int new_state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE,
                    BluetoothProfile.STATE_DISCONNECTED);
                int prev_state = intent.getIntExtra(BluetoothProfile.EXTRA_PREVIOUS_STATE,
                    BluetoothProfile.STATE_DISCONNECTED);
                Log.d(TAG, "ACTION_CONNECTION_STATE_CHANGED, new_state = " + new_state);
                if (new_state == BluetoothProfile.STATE_CONNECTED){
                    mConnectedDevice = (BluetoothDevice)intent.getParcelableExtra(
                        BluetoothDevice.EXTRA_DEVICE);
                    mPbapClientProfileState = BluetoothProfile.STATE_CONNECTED;
                    Log.d(TAG, "onReceive, mPbapClientProfileState = " + mPbapClientProfileState);
                } else {
                    mConnectedDevice = null;
                    mPbapClientProfileState = BluetoothProfile.STATE_DISCONNECTED;
                    Log.d(TAG, "onReceive 2, mPbapClientProfileState = " + mPbapClientProfileState);
                }
            }
        }
    };

    private void registerReceiver(){
        IntentFilter filter = new IntentFilter();
        filter.addAction(PhoneCallActivity.DESTORY_MSG);
        filter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        filter.addAction(BluetoothPbapClient.ACTION_CONNECTION_STATE_CHANGED);
        getActivity().registerReceiver(mReceiver, filter);
    }

    public void cleanData(){
       Message clear = mHandler.obtainMessage(MESSAGE_CLEAR_DATA,
                RECHECK_PATH_DELAY);
       mHandler.sendMessage(clear);
    }

}
