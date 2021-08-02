package com.mediatek.location.lppe.lbs;

import android.content.Context;
import android.location.Location;
import android.os.Bundle;
import android.os.SystemProperties;
import android.util.Log;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;

public class GooglePlayLocManager {

    private final static String TAG = "GooglePlayLocManager";

    private Context mContext;
    private GoogleApiClient mGoogleApiClient;
    private LocationRequest mLocationRequest;
    private boolean mGmsConnected;
    private GoogleApiListener mGoogleApiListener;
    private android.location.LocationListener mAospListener;
    private boolean mGmsInstalled;

    public GooglePlayLocManager(Context context) {
        mContext = context;
        mGoogleApiListener = new GoogleApiListener();
        mGoogleApiClient = new GoogleApiClient.Builder(mContext)
                .addConnectionCallbacks(mGoogleApiListener)
                .addOnConnectionFailedListener(mGoogleApiListener)
                .addApi(LocationServices.API)
                .build();
        mGmsInstalled = checkGmsInstalled();
    }

    public boolean isGmsInstalled() {
        Log.d(TAG, "isGmsInstalled() = " + mGmsInstalled);
        return mGmsInstalled;
    }

    private boolean checkGmsInstalled() {
        String gms = SystemProperties.get("ro.com.google.gmsversion", null);
        Log.d(TAG, "gmsLoadCheck ro.com.google.gmsversion: " + gms);
        if (gms == null || gms.length() == 0) {
            return false;
        }
        return true;
    }

    private class GoogleApiListener implements GoogleApiClient.ConnectionCallbacks,
            GoogleApiClient.OnConnectionFailedListener {

        @Override
        public void onConnected(Bundle bundle) {
            Log.d(TAG, "onConnected");
            mGmsConnected = true;
            if (mAospListener != null) {
                mLocationRequest = LocationRequest.create();
                mLocationRequest.setInterval(1000);
                requestLocationUpdatesImpl();
            } else {
                disconnectGooglePlay();
            }
        }

        @Override
        public void onConnectionSuspended(int i) {
            Log.d(TAG, "Connection Suspended cause " + i);
            mGmsConnected = false;
        }

        @Override
        public void onConnectionFailed(ConnectionResult connectionResult) {
            Log.d(TAG, "Connection failed. Error: " + connectionResult.getErrorCode());
            mGmsConnected = false;
        }
    }

    private void disconnectGooglePlay() {
        Log.i(TAG, "disconnectGooglePlay()");
        if (mGoogleApiClient.isConnected()) {
            mGoogleApiClient.disconnect();
        }
        mGmsConnected = false;
    }

    private void connectGooglePlay() {
        Log.d(TAG, "connectGooglePlay()");
        if (!mGoogleApiClient.isConnected()) {
            mGoogleApiClient.connect();
        } else {
            mGmsConnected = true;
        }
    }

    private LocationListener mGmsLocationListener = new LocationListener() {

        @Override
        public void onLocationChanged(Location location) {
            if (location == null) {
                Log.d(TAG, "ERR: onLocationChanged get null location");
            } else if (mAospListener == null) {
                Log.d(TAG, "ERR: onLocationChanged get null listener");
                removeListeners(null);
            } else {
                Log.d(TAG, "onLocationChanged");
                mAospListener.onLocationChanged(location);
            }
        }
    };

    public void requestLocationUpdates(android.location.LocationListener listener) {
        Log.d(TAG, "requestLocationUpdates mGmsConnected = " + mGmsConnected);
        mAospListener = listener;
        if (mGmsConnected) {
            requestLocationUpdatesImpl();
        } else {
            connectGooglePlay();
        }
    }


    private void requestLocationUpdatesImpl() {
        Log.d(TAG, "requestLocationUpdatesImpl");
        try {
            LocationServices.FusedLocationApi.requestLocationUpdates(
                    mGoogleApiClient, mLocationRequest, mGmsLocationListener);
        } catch (IllegalStateException e) {
            Log.d(TAG, "Failed requestLocationUpdatesImpl err:", e);
        }
    }

    public void removeListeners(android.location.LocationListener listener) {
        Log.d(TAG, "removeListeners");
        try {
            mAospListener = null;
            LocationServices.FusedLocationApi.removeLocationUpdates(
                mGoogleApiClient, mGmsLocationListener);
        } catch (IllegalStateException e) {
            Log.d(TAG, "Failed removeListeners err:", e);
        }
    }

}
