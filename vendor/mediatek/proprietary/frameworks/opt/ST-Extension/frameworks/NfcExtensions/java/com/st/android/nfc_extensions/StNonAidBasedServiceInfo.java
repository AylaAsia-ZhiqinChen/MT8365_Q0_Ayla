package com.st.android.nfc_extensions;

import android.util.Log;

public class StNonAidBasedServiceInfo {
    static final String TAG = "APINfc_StNonAidBasedServiceInfo";
    static final boolean DBG = true;

    int mSeId;

    public StNonAidBasedServiceInfo(int host_id) {
        if (DBG) Log.d(TAG, "Constructor - hostId: " + host_id);

        this.mSeId = host_id;
    }

    public int getSeId() {
        if (DBG) Log.d(TAG, "getHostId - seId: " + this.mSeId);
        return this.mSeId;
    }
}
