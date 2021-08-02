package com.mediatek.bluetooth.pbapclient;


//import android.bluetooth.client.pbap.BluetoothPbapCard;
import android.bluetooth.BluetoothPbapClient;

import com.android.vcard.VCardEntry;

import java.util.ArrayList;

public interface BluetoothPabapClientCallback {
    
    void onConnectStatusChange(boolean connected);

    void onSetPathDone(boolean success);
    
    void onPullBookDone(boolean success, int newMissedCalls, ArrayList<VCardEntry> list);
    
    void onPullVcardListingDone(boolean success, int newMissedCalls, ArrayList</*BluetoothPbapCard*/String> list);
    
    void onPullVcardEntryDone(boolean success, VCardEntry entry);
    
    void onPullPhonebookSizeDone(boolean success, int size);
    
    void onPullVcardListingSizeDone(boolean success, int size);

    void onAuthenticationRequest();

    void onAuthenticationTimeout();

}
