/*
 * Copyright (C) 2008 The Android Open Source Project
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

package android.os.cts;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Process;
import java.io.File;
import java.io.IOException;

public class CtsRemoteService extends Service {

    @Override
    public void onCreate() {
        super.onCreate();
        android.util.Log.d("Process test stub", "CtsRemoteServiceProcessPid:" + Process.myPid());
    }

    private final ISecondary.Stub mSecondaryBinder = new ISecondary.Stub() {
        public int getPid() {
            return Process.myPid();
        }

        public long getElapsedCpuTime() {
            return Process.getElapsedCpuTime();
        }

        public String getTimeZoneID() {
            return java.util.TimeZone.getDefault().getID();
        }

        public boolean performDiskWrite() {
            try {
                File tempFile = File.createTempFile("foo", "bar");
                return tempFile.delete();
            } catch (IOException exception) {
                return false;
            }
        }
    };

    @Override
    public IBinder onBind(Intent intent) {
        if (ISecondary.class.getName().equals(intent.getAction())) {
            return mSecondaryBinder;
        }
        return null;
    }
}
