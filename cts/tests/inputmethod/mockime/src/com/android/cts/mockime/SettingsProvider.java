/*
 * Copyright (C) 2018 The Android Open Source Project
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

package com.android.cts.mockime;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;

import androidx.annotation.Nullable;

/**
 * {@link ContentProvider} to receive {@link ImeSettings} via
 * {@link ContentProvider#call(String, String, String, Bundle)}.
 */
public class SettingsProvider extends ContentProvider {

    static final String AUTHORITY = "com.android.cts.mockime.provider";

    @Nullable
    private static ImeSettings sSettings = null;

    @Override
    public boolean onCreate() {
        return true;
    }

    @Override
    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs,
            String sortOrder) {
        return null;
    }

    @Override
    public String getType(Uri uri) {
        return null;
    }

    @Override
    public Uri insert(Uri uri, ContentValues values) {
        return null;
    }

    @Override
    public int delete(Uri uri, String selection, String[] selectionArgs) {
        return 0;
    }

    @Override
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs) {
        return 0;
    }

    @Override
    public Bundle call(String authority, String method, String arg, Bundle extras) {
        if ("write".equals(method)) {
            sSettings = null;
            final String callingPackageName = getCallingPackage();
            if (callingPackageName == null) {
                throw new SecurityException("Failed to obtain the calling package name.");
            }
            sSettings = new ImeSettings(callingPackageName, extras);
        } else if ("delete".equals(method)) {
            sSettings = null;
        }
        return Bundle.EMPTY;
    }

    static ImeSettings getSettings() {
        return sSettings;
    }
}
