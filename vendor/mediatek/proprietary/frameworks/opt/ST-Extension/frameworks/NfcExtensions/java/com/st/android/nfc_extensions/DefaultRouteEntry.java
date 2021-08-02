package com.st.android.nfc_extensions;

import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;

public class DefaultRouteEntry implements Parcelable {
    String routeName;
    String routeLoc;

    private static final String TAG = "Nfc_DefaultRouteEntry";

    public DefaultRouteEntry(String name, String loc) {
        Log.d(TAG, "DefaultRouteEntry(constructor) - name: " + name + ", loc: " + loc);
        routeName = name;
        routeLoc = loc;
    }

    public String getRouteName() {
        return this.routeName;
    }

    public String getRouteLoc() {
        return this.routeLoc;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(routeName);
        dest.writeString(routeLoc);
    }

    public static final Parcelable.Creator<DefaultRouteEntry> CREATOR =
            new Parcelable.Creator<DefaultRouteEntry>() {
                @Override
                public DefaultRouteEntry createFromParcel(Parcel source) {
                    String name = source.readString();
                    String loc = source.readString();

                    return new DefaultRouteEntry(name, loc);
                }

                @Override
                public DefaultRouteEntry[] newArray(int size) {
                    return new DefaultRouteEntry[size];
                }
            };
}
