/*
 * Copyright (C) 2010-2016 Orange.
 * Copyright (C) 2014 Sony Mobile Communications Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package com.orangelabs.rcs.provider.sharing;

import com.orangelabs.rcs.provider.CursorUtil;
import com.orangelabs.rcs.provider.history.HistoryMemberBaseIdCreator;
import com.orangelabs.rcs.service.api.ServerApiPersistentStorageException;
import com.orangelabs.rcs.utils.DatabaseUtils;
import com.gsma.services.rcs.history.HistoryLog;
import com.gsma.services.rcs.sharing.geoloc.GeolocSharingLog;

import android.content.ContentProvider;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.net.Uri;
//import android.support.annotation.NonNull;
import android.text.TextUtils;

@SuppressWarnings("ConstantConditions")
public class GeolocSharingProvider extends ContentProvider {

    private static final int INVALID_ROW_ID = -1;

    private static final String SELECTION_WITH_SHARING_ID_ONLY = GeolocSharingData.KEY_SHARING_ID
            .concat("=?");

    private static final class UriType {

        private static final class GeolocSharing {
            private static final int BASE = 1;

            private static final int WITH_SHARING_ID = 2;
        }

        private static final class InternalGeolocSharing {
            private static final int BASE = 3;

            private static final int WITH_SHARING_ID = 4;
        }
    }

    private static final class CursorType {

        private static final String TYPE_DIRECTORY = "vnd.android.cursor.dir/geolocshare";

        private static final String TYPE_ITEM = "vnd.android.cursor.item/geolocshare";
    }

    private static final UriMatcher sUriMatcher;
    static {
        sUriMatcher = new UriMatcher(UriMatcher.NO_MATCH);
        sUriMatcher.addURI(GeolocSharingData.CONTENT_URI.getAuthority(),
                GeolocSharingData.CONTENT_URI.getPath().substring(1),
                UriType.InternalGeolocSharing.BASE);
        sUriMatcher.addURI(GeolocSharingData.CONTENT_URI.getAuthority(),
                GeolocSharingData.CONTENT_URI.getPath().substring(1).concat("/*"),
                UriType.InternalGeolocSharing.WITH_SHARING_ID);
        sUriMatcher.addURI(GeolocSharingLog.CONTENT_URI.getAuthority(),
                GeolocSharingLog.CONTENT_URI.getPath().substring(1), UriType.GeolocSharing.BASE);
        sUriMatcher.addURI(GeolocSharingLog.CONTENT_URI.getAuthority(),
                GeolocSharingLog.CONTENT_URI.getPath().substring(1).concat("/*"),
                UriType.GeolocSharing.WITH_SHARING_ID);
    }

    /**
     * Table name
     */
    public static final String TABLE = "geolocshare";

    /**
     * Database name
     */
    public static final String DATABASE_NAME = "geolocshare.db";

    private static class DatabaseHelper extends SQLiteOpenHelper {
        private static final int DATABASE_VERSION = 3;

        public DatabaseHelper(Context ctx) {
            super(ctx, DATABASE_NAME, null, DATABASE_VERSION);
        }

        @Override
        public void onCreate(SQLiteDatabase db) {
            // @formatter:off
            db.execSQL("CREATE TABLE IF NOT EXISTS " + TABLE + " ("
                    + GeolocSharingData.KEY_SHARING_ID + " TEXT NOT NULL PRIMARY KEY,"
                    + GeolocSharingData.KEY_BASECOLUMN_ID + " INTEGER NOT NULL,"
                    + GeolocSharingData.KEY_CONTACT + " TEXT NOT NULL,"
                    + GeolocSharingData.KEY_CONTENT + " TEXT," 
                    + GeolocSharingData.KEY_MIME_TYPE + " TEXT NOT NULL," 
                    + GeolocSharingData.KEY_DIRECTION + " INTEGER NOT NULL,"
                    + GeolocSharingData.KEY_STATE + " INTEGER NOT NULL,"
                    + GeolocSharingData.KEY_REASON_CODE + " INTEGER NOT NULL,"
                    + GeolocSharingData.KEY_TIMESTAMP + " INTEGER NOT NULL)");
            // @formatter:on
            db.execSQL("CREATE INDEX " + GeolocSharingData.KEY_BASECOLUMN_ID + "_idx ON " + TABLE
                    + '(' + GeolocSharingData.KEY_BASECOLUMN_ID + ')');
            db.execSQL("CREATE INDEX " + GeolocSharingData.KEY_CONTACT + "_idx ON " + TABLE + '('
                    + GeolocSharingData.KEY_CONTACT + ')');
            db.execSQL("CREATE INDEX " + GeolocSharingData.KEY_TIMESTAMP + "_idx ON " + TABLE + '('
                    + GeolocSharingData.KEY_TIMESTAMP + ')');
        }

        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int currentVersion) {
            db.execSQL("DROP TABLE IF EXISTS ".concat(TABLE));
            onCreate(db);
        }
    }

    private SQLiteOpenHelper mOpenHelper;

    private String getSelectionWithSharingId(String selection) {
        if (TextUtils.isEmpty(selection)) {
            return SELECTION_WITH_SHARING_ID_ONLY;
        }
        return "(" + SELECTION_WITH_SHARING_ID_ONLY + ") AND (" + selection + ')';
    }

    private String[] getSelectionArgsWithSharingId(String[] selectionArgs, String sharingId) {
        return DatabaseUtils.appendIdWithSelectionArgs(sharingId, selectionArgs);
    }

    @Override
    public boolean onCreate() {
        mOpenHelper = new DatabaseHelper(getContext());
        return true;
    }

    @Override
    public String getType( Uri uri) {
        switch (sUriMatcher.match(uri)) {
            case UriType.InternalGeolocSharing.BASE:
                /* Intentional fall through */
            case UriType.GeolocSharing.BASE:
                return CursorType.TYPE_DIRECTORY;

            case UriType.InternalGeolocSharing.WITH_SHARING_ID:
                /* Intentional fall through */
            case UriType.GeolocSharing.WITH_SHARING_ID:
                return CursorType.TYPE_ITEM;

            default:
                throw new IllegalArgumentException("Unsupported URI " + uri + "!");
        }
    }

    @Override
    public Cursor query( Uri uri, String[] projection, String selection,
            String[] selectionArgs, String sort) {
        Cursor cursor = null;
        try {
            switch (sUriMatcher.match(uri)) {
                case UriType.InternalGeolocSharing.WITH_SHARING_ID:
                    String sharingId = uri.getLastPathSegment();
                    selection = getSelectionWithSharingId(selection);
                    selectionArgs = getSelectionArgsWithSharingId(selectionArgs, sharingId);
                    SQLiteDatabase db = mOpenHelper.getReadableDatabase();
                    cursor = db
                            .query(TABLE, projection, selection, selectionArgs, null, null, sort);
                    CursorUtil.assertCursorIsNotNull(cursor, uri);
                    cursor.setNotificationUri(getContext().getContentResolver(),
                            Uri.withAppendedPath(GeolocSharingLog.CONTENT_URI, sharingId));
                    return cursor;

                case UriType.InternalGeolocSharing.BASE:
                    db = mOpenHelper.getReadableDatabase();
                    cursor = db
                            .query(TABLE, projection, selection, selectionArgs, null, null, sort);
                    CursorUtil.assertCursorIsNotNull(cursor, uri);
                    cursor.setNotificationUri(getContext().getContentResolver(),
                            GeolocSharingLog.CONTENT_URI);
                    return cursor;

                case UriType.GeolocSharing.WITH_SHARING_ID:
                    sharingId = uri.getLastPathSegment();
                    selection = getSelectionWithSharingId(selection);
                    selectionArgs = getSelectionArgsWithSharingId(selectionArgs, sharingId);
                    /* Intentional fall through */
                    //$FALL-THROUGH$
                case UriType.GeolocSharing.BASE:
                    db = mOpenHelper.getReadableDatabase();
                    cursor = db
                            .query(TABLE, projection, selection, selectionArgs, null, null, sort);
                    CursorUtil.assertCursorIsNotNull(cursor, uri);
                    cursor.setNotificationUri(getContext().getContentResolver(), uri);
                    return cursor;

                default:
                    throw new IllegalArgumentException("Unsupported URI " + uri + "!");
            }
        }
        /*
         * TODO: Do not catch, close cursor, and then throw same exception. Callers should handle
         * exception.
         */
        catch (RuntimeException e) {
            if (cursor != null) {
                cursor.close();
            }
            throw e;
        }
    }

    @Override
    public int update( Uri uri, ContentValues values, String selection,
            String[] selectionArgs) {
        Uri notificationUri = GeolocSharingLog.CONTENT_URI;
        switch (sUriMatcher.match(uri)) {
            case UriType.InternalGeolocSharing.WITH_SHARING_ID:
                String sharingId = uri.getLastPathSegment();
                selection = getSelectionWithSharingId(selection);
                selectionArgs = getSelectionArgsWithSharingId(selectionArgs, sharingId);
                notificationUri = Uri.withAppendedPath(notificationUri, sharingId);
                /* Intentional fall through */
                //$FALL-THROUGH$
            case UriType.InternalGeolocSharing.BASE:
                SQLiteDatabase db = mOpenHelper.getWritableDatabase();
                int count = db.update(TABLE, values, selection, selectionArgs);
                if (count > 0) {
                    ContentResolver resolver = getContext().getContentResolver();
                    resolver.notifyChange(notificationUri, null);
                    resolver.notifyChange(HistoryLog.CONTENT_URI, null);
                }
                return count;

            case UriType.GeolocSharing.WITH_SHARING_ID:
                /* Intentional fall through */
            case UriType.GeolocSharing.BASE:
                throw new UnsupportedOperationException("This provider (URI=" + uri
                        + ") supports read only access!");

            default:
                throw new IllegalArgumentException("Unsupported URI " + uri + "!");
        }
    }

    @Override
    public Uri insert( Uri uri, ContentValues initialValues) {
        switch (sUriMatcher.match(uri)) {
            case UriType.InternalGeolocSharing.WITH_SHARING_ID:
                /* Intentional fall through */
            case UriType.InternalGeolocSharing.BASE:
                SQLiteDatabase db = mOpenHelper.getWritableDatabase();
                String sharingId = initialValues.getAsString(GeolocSharingData.KEY_SHARING_ID);
                initialValues.put(GeolocSharingData.KEY_BASECOLUMN_ID, HistoryMemberBaseIdCreator
                        .createUniqueId(getContext(), GeolocSharingData.HISTORYLOG_MEMBER_ID));
                if (db.insert(TABLE, null, initialValues) == INVALID_ROW_ID) {
                    throw new ServerApiPersistentStorageException("Unable to insert row for URI "
                            + uri.toString() + '!');
                }
                Uri notificationUri = GeolocSharingLog.CONTENT_URI.buildUpon()
                        .appendPath(sharingId).build();
                ContentResolver resolver = getContext().getContentResolver();
                resolver.notifyChange(notificationUri, null);
                resolver.notifyChange(HistoryLog.CONTENT_URI, null);
                return notificationUri;

            case UriType.GeolocSharing.WITH_SHARING_ID:
                /* Intentional fall through */
            case UriType.GeolocSharing.BASE:
                throw new UnsupportedOperationException("This provider (URI=" + uri
                        + ") supports read only access!");

            default:
                throw new IllegalArgumentException("Unsupported URI " + uri + "!");
        }
    }

    @Override
    public int delete( Uri uri, String selection, String[] selectionArgs) {
        Uri notificationUri = GeolocSharingLog.CONTENT_URI;
        switch (sUriMatcher.match(uri)) {
            case UriType.InternalGeolocSharing.WITH_SHARING_ID:
                String sharingId = uri.getLastPathSegment();
                selection = getSelectionWithSharingId(selection);
                selectionArgs = getSelectionArgsWithSharingId(selectionArgs, sharingId);
                notificationUri = Uri.withAppendedPath(notificationUri, sharingId);
                /* Intentional fall through */
                //$FALL-THROUGH$
            case UriType.InternalGeolocSharing.BASE:
                SQLiteDatabase db = mOpenHelper.getWritableDatabase();
                int count = db.delete(TABLE, selection, selectionArgs);
                if (count > 0) {
                    ContentResolver resolver = getContext().getContentResolver();
                    resolver.notifyChange(notificationUri, null);
                    resolver.notifyChange(HistoryLog.CONTENT_URI, null);
                }
                return count;

            case UriType.GeolocSharing.WITH_SHARING_ID:
                /* Intentional fall through */
            case UriType.GeolocSharing.BASE:
                throw new UnsupportedOperationException("This provider (URI=" + uri
                        + ") supports read only access!");

            default:
                throw new IllegalArgumentException("Unsupported URI " + uri + "!");
        }
    }

}
