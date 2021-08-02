/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.android.settings.deviceinfo;

import android.content.Context;

import android.util.Log;
import androidx.preference.Preference;
import androidx.preference.PreferenceScreen;
import com.android.settings.core.BasePreferenceController;
import android.content.pm.PackageInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;

public class FotaUpdatePreferenceController extends BasePreferenceController {

    private static final String KEY_FOTA_UPDATE = "fota_update_settings";
    private static final String FOTA_PACKAGE_NAME = "com.adups.fota";
    private Preference mPreference;

    public FotaUpdatePreferenceController(Context context, String key) {
        super(context, key);
    }

    private boolean isApkExist(String packageName){
        PackageManager pm = mContext.getPackageManager();
        PackageInfo pInfo = null;
        try{
            pInfo = pm.getPackageInfo(packageName,PackageManager.GET_ACTIVITIES);
            Log.e("FotaUpdate","Fota apk found..");
        }catch(PackageManager.NameNotFoundException e){
            Log.e("FotaUpdate","Fota apk not found..");
            return false;
        }catch(Exception xe){
            return false;
        }
        return true;
    }

    private String getApkName(String packageName){
        PackageManager pm = mContext.getPackageManager();
        ApplicationInfo aInfo = null;
        try{
            aInfo = pm.getApplicationInfo(packageName,PackageManager.GET_ACTIVITIES);
        }catch(PackageManager.NameNotFoundException e){
            Log.e("FotaUpdate","Fota apk not found..");
        }catch(Exception xe){
            aInfo = null;
        }
        return (String)pm.getApplicationLabel(aInfo);
    }

    private String getApkVersion(String packageName){
        PackageManager pm = mContext.getPackageManager();
        PackageInfo pInfo = null;
        try {
            pInfo = pm.getPackageInfo(packageName, 0);
            return (pInfo != null) ? pInfo.versionName : "";
        }catch(Exception xe){
            pInfo = null;
        }
        return "";
    }


    @Override
    public void displayPreference(PreferenceScreen screen) {
        mPreference = screen.findPreference(getPreferenceKey());
        super.displayPreference(screen);

      if (getAvailabilityStatus() == AVAILABLE) {
      String title = getApkName(FOTA_PACKAGE_NAME);
        if(title != null&& !title.equals("")){
          mPreference.setTitle(title);
          Log.e("FotaUpdate","preference  set preference title :" + title);
        }else{
          Log.e("FotaUpdate","preference  set preference title null");
        }
        }
    }

    @Override
    public int getAvailabilityStatus() {
      String packageName = FOTA_PACKAGE_NAME;
      String versionName = getApkVersion(packageName);
      String[] names = versionName.split("\\.");

      if (!isApkExist(packageName)){		
        return DISABLED_FOR_USER;
      }else if (names.length >= 4 && ("9".equals(names[3]) || "9".equals(names[2]))){
        return DISABLED_FOR_USER;
      }else{
        return AVAILABLE;
      }
    }
}
