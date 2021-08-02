package com.android.settings.ethernet;

import android.content.Context;
import androidx.preference.SwitchPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceScreen;
import android.provider.Settings;
import com.android.settings.core.PreferenceControllerMixin;
import com.android.settingslib.core.AbstractPreferenceController;
import com.android.settingslib.core.lifecycle.LifecycleObserver;

public class EthernetPreferenceController extends AbstractPreferenceController
        implements PreferenceControllerMixin, LifecycleObserver {

    private static final String KEY_ETHERNET_SETTINGS = "ethernet_settings";

    public EthernetPreferenceController(Context context) {
        super(context);
    }

    @Override
    public String getPreferenceKey() {
        return KEY_ETHERNET_SETTINGS;
    }

    @Override
    public boolean isAvailable() {
        if (android.os.SystemProperties.getBoolean("persist.sys.add.ethernet.settings", false)) {
            /*if (android.os.SystemProperties.get("persist.sys.exist.ethernet", "0").equals("1")) {
                return true;
            } else {
                return false;
            }*/
            return true;
        } else {
            return false;
        }
    }
}
