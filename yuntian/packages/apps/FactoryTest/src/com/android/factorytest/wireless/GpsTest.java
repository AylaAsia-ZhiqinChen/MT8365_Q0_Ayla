package com.android.factorytest.wireless;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.ContentResolver;
import android.content.Context;
import android.database.ContentObservable;
import android.database.ContentObserver;
import android.location.GpsSatellite;
import android.location.GpsStatus;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * 定位测试
 */
public class GpsTest extends BaseActivity {

    private static final int MSG_UPDATE_LOCATION_TIME = 0;

    private TextView mGPSStateTv;
    private ListView mSatelliteListView;
    private TextView mEmptyView;
    private TextView mLocationTimeUseTv;
    private LocationManager mLocationManager;
    private SatelliteListAdapter mAdapter;

    private int mLocationTime;

    private boolean mLastGpsEnabled;
    private int mOriginalMode;

    @SuppressLint("MissingPermission")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_gps_test);

        super.onCreate(savedInstanceState);

        mLocationManager = (LocationManager) getSystemService(LOCATION_SERVICE);
        initEmptyView();
        mLocationTime = 0;
        mLastGpsEnabled = isLocationEnabled();
        mOriginalMode = Settings.Secure.getInt(getContentResolver(), Settings.Secure.LOCATION_MODE, Settings.Secure.LOCATION_MODE_OFF);//add by yt_wxc for not changing the GPS setting after factory test
        mAdapter = new SatelliteListAdapter(this, new ArrayList<GpsSatellite>());
        getContentResolver().registerContentObserver(Settings.Secure.getUriFor(Settings.Secure.LOCATION_MODE), true, mGpsObserver);

        mGPSStateTv = (TextView) findViewById(R.id.gps_state);
        mLocationTimeUseTv = (TextView) findViewById(R.id.time_cost);
        mSatelliteListView = (ListView) findViewById(R.id.gps_list);

        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        lp.gravity = Gravity.CENTER;
        ((ViewGroup) mSatelliteListView.getParent()).addView(mEmptyView, lp);
        mSatelliteListView.setEmptyView(mEmptyView);
        mSatelliteListView.setAdapter(mAdapter);
        mEmptyView.setText(R.string.empty_satellite_list);

        Log.d(this, "onCreate=>enabled: " + isLocationEnabled());
        mLocationTimeUseTv.setText(getResources().getString(R.string.location_use_time, mLocationTime));
        if (!isLocationEnabled()) {
            mGPSStateTv.setText(R.string.gps_close_title);
            boolean result = setLocationEnabled(true);
            Log.d(this, "onCreate=>enabled location result: " + result);
            if (result) {
                mGPSStateTv.setText(R.string.gps_open_title);
                mLocationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 0, mLocationListener);
                mLocationManager.addGpsStatusListener(mGpsStatusListener);
                mEmptyView.setText(R.string.searching_satellite);
                mHandler.removeMessages(MSG_UPDATE_LOCATION_TIME);
                mHandler.sendEmptyMessage(MSG_UPDATE_LOCATION_TIME);
            }
        } else {
            mGPSStateTv.setText(R.string.gps_open_title);
            mLocationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 0, mLocationListener);
            mLocationManager.addGpsStatusListener(mGpsStatusListener);
            mHandler.removeMessages(MSG_UPDATE_LOCATION_TIME);
            mHandler.sendEmptyMessage(MSG_UPDATE_LOCATION_TIME);
            mEmptyView.setText(R.string.searching_satellite);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mLocationManager.removeUpdates(mLocationListener);
        mLocationManager.removeGpsStatusListener(mGpsStatusListener);
        getContentResolver().unregisterContentObserver(mGpsObserver);
        //add by yt_wxc for not changing the GPS setting after factory test begin
       // if (!mLastGpsEnabled) {
         //   setLocationEnabled(false);
       // }
       Settings.Secure.putInt(getContentResolver(), Settings.Secure.LOCATION_MODE, mOriginalMode);
       //add by yt_wxc for not changing the GPS setting after factory test end
    }

    private void initEmptyView() {
        mEmptyView = new TextView(this);
        mEmptyView.setTextSize(TypedValue.COMPLEX_UNIT_SP, getResources().getInteger(R.integer.wifi_list_empty_view_text_size));
        mEmptyView.setGravity(Gravity.CENTER);
    }

    public boolean setLocationEnabled(boolean enabled) {
        final ContentResolver cr = getContentResolver();
        // When enabling location, a user consent dialog will pop up, and the
        // setting won't be fully enabled until the user accepts the agreement.
        int mode = enabled
                ? Settings.Secure.LOCATION_MODE_HIGH_ACCURACY : Settings.Secure.LOCATION_MODE_OFF;
        // QuickSettings always runs as the owner, so specifically set the settings
        // for the current foreground user.
        return Settings.Secure
                .putInt(cr, Settings.Secure.LOCATION_MODE, mode);
    }

    public boolean isLocationEnabled() {
        ContentResolver resolver = getContentResolver();
        // QuickSettings always runs as the owner, so specifically retrieve the settings
        // for the current foreground user.
        int mode = Settings.Secure.getInt(resolver, Settings.Secure.LOCATION_MODE,
                Settings.Secure.LOCATION_MODE_OFF);
        Log.d(this, "isLocationEnabled=>mode: " + mode);
        return mode == Settings.Secure.LOCATION_MODE_HIGH_ACCURACY;
    }

    private void updateLocationUseTime() {
        mLocationTime++;
        mLocationTimeUseTv.setText(getResources().getString(R.string.location_use_time, mLocationTime));
    }

    private LocationListener mLocationListener = new LocationListener() {
        @Override
        public void onLocationChanged(Location location) {

        }

        @Override
        public void onStatusChanged(String s, int i, Bundle bundle) {

        }

        @Override
        public void onProviderEnabled(String s) {

        }

        @Override
        public void onProviderDisabled(String s) {

        }
    };

    private GpsStatus.Listener mGpsStatusListener = new GpsStatus.Listener() {
        @Override
        @SuppressLint("MissingPermission")
        public void onGpsStatusChanged(int event) {
            if (event == GpsStatus.GPS_EVENT_SATELLITE_STATUS) {
                ArrayList<GpsSatellite> satellites = new ArrayList<GpsSatellite>();
                Iterator<GpsSatellite> iterable = mLocationManager.getGpsStatus(null).getSatellites().iterator();
                while (iterable.hasNext()) {
                    GpsSatellite satellite = iterable.next();
                    if (satellite != null && satellite.getSnr() > 0.1) {
                        satellites.add(satellite);
                    }
                }
                //yuntian longyao add
                //Description:工厂模式里面搜星必须在3个以上才算通过
                if (satellites.size() > 2) {
                    setPassButtonEnabled(true);
                    setTestPass(true);
                    if (isAutoTest()) {
                        if (isTestPass()) {
                            doAtOnceOnAutoTest();
                        }
                    }
                }
                //yuntian longyao add
                mAdapter.setList(satellites);
            }
        }
    };

    private ContentObserver mGpsObserver = new ContentObserver(new Handler()) {
        @SuppressLint("MissingPermission")
        @Override
        public void onChange(boolean selfChange, Uri uri) {
            Log.d(this, "onChange=>uri: " + uri + " enabled: " + isLocationEnabled());
            if (Settings.Secure.getUriFor(Settings.Secure.LOCATION_MODE).equals(uri)) {
                Log.d(this, "onChange=>enabled: " + isLocationEnabled());
                if (isLocationEnabled()) {
                    mGPSStateTv.setText(R.string.gps_open_title);
                    mLocationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 0, mLocationListener);
                    mLocationManager.addGpsStatusListener(mGpsStatusListener);
                    mEmptyView.setText(R.string.searching_satellite);
                    mHandler.removeMessages(MSG_UPDATE_LOCATION_TIME);
                    mHandler.sendEmptyMessage(MSG_UPDATE_LOCATION_TIME);
                } else {
                    mGPSStateTv.setText(R.string.gps_close_title);
                    mEmptyView.setText(R.string.empty_satellite_list);
                    mHandler.removeMessages(MSG_UPDATE_LOCATION_TIME);
                    mLocationTime = 0;
                    mLocationTimeUseTv.setText(getResources().getString(R.string.location_use_time, mLocationTime));
                }
            }
        }
    };

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_UPDATE_LOCATION_TIME:
                    updateLocationUseTime();
                    mHandler.sendEmptyMessageDelayed(MSG_UPDATE_LOCATION_TIME, 1000);
                    break;
            }
        }
    };

    private class SatelliteListAdapter extends BaseAdapter {

        private Context mContext;
        private LayoutInflater mInflater;
        private List<GpsSatellite> mList;
        private DecimalFormat mDecimalFormat;

        public  SatelliteListAdapter(Context context, List<GpsSatellite> list) {
            mContext = context;
            mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mList = list;
            mDecimalFormat = new DecimalFormat("#.00");
        }

        public void setList(List<GpsSatellite> list) {
            mList = null;
            mList = list;
            notifyDataSetChanged();
        }

        @Override
        public int getCount() {
            return (mList != null ? mList.size() : 0);
        }

        @Override
        public GpsSatellite getItem(int i) {
            return mList.get(i);
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {
            ViewHolder holder;
            if (view == null) {
                view = mInflater.inflate(R.layout.satellite_item, viewGroup, false);
                holder = new ViewHolder();
                holder.satelliteName = (TextView) view.findViewById(R.id.satellite_name);
                holder.satelliteSnr = (TextView) view.findViewById(R.id.satellite_snr);
                view.setTag(holder);
            } else {
                holder = (ViewHolder) view.getTag();
            }
            GpsSatellite satellite = getItem(i);
            holder.satelliteName.setText(mContext.getString(R.string.satellite_id, satellite.getPrn()));
            holder.satelliteSnr.setText(mContext.getString(R.string.satellite_snr_title, mDecimalFormat.format((double)satellite.getSnr())));
            return view;
        }

        class ViewHolder {
            TextView satelliteName;
            TextView satelliteSnr;
        }
    }
}
