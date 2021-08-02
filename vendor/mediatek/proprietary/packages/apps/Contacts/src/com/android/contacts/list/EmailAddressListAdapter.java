/*
 * Copyright (C) 2011 The Android Open Source Project
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
package com.android.contacts.list;

import android.content.ContentUris;
import android.content.Context;
import android.content.CursorLoader;
import android.database.Cursor;
import android.net.Uri;
import android.net.Uri.Builder;
import android.provider.ContactsContract;
import android.provider.ContactsContract.CommonDataKinds.Email;
import android.provider.ContactsContract.Data;
import android.text.TextUtils;
import android.view.View;
import android.view.ViewGroup;

import com.android.contacts.ContactPhotoManager.DefaultImageRequest;
import com.android.contacts.preference.ContactsPreferences;

import com.google.common.collect.Lists;

import com.mediatek.contacts.util.ContactsPortableUtils;
import com.mediatek.contacts.util.Log;
import com.mediatek.provider.MtkContactsContract;

import java.util.List;

/**
 * A cursor adapter for the {@link Email#CONTENT_TYPE} content type.
 */
public class EmailAddressListAdapter extends ContactEntryListAdapter {
    private static final String TAG = "EmailAddressListAdapter";

    protected static class EmailQuery {
        private static final String[] PROJECTION_PRIMARY_INTERNAL = new String[] {
            Email._ID,                       // 0
            Email.TYPE,                      // 1
            Email.LABEL,                     // 2
            Email.DATA,                      // 3
            Email.PHOTO_ID,                  // 4
            Email.LOOKUP_KEY,                // 5
            Email.DISPLAY_NAME_PRIMARY,      // 6
        };
        ///M:[Sim Contact Flow] @{
        private static final String[] PROJECTION_PRIMARY;
        static {
            List<String> projectionList = Lists.newArrayList(PROJECTION_PRIMARY_INTERNAL);
            if (ContactsPortableUtils.MTK_PHONE_BOOK_SUPPORT) {
                projectionList.add(MtkContactsContract.RawContactsColumns.INDICATE_PHONE_SIM); //7
                projectionList.add(MtkContactsContract.RawContactsColumns.IS_SDN_CONTACT); //8
            }
            PROJECTION_PRIMARY = projectionList.toArray(new String[projectionList.size()]);
        }
        /// @}

        private static final String[] PROJECTION_ALTERNATIVE_INTERNAL = new String[] {
            Email._ID,                       // 0
            Email.TYPE,                      // 1
            Email.LABEL,                     // 2
            Email.DATA,                      // 3
            Email.PHOTO_ID,                  // 4
            Email.LOOKUP_KEY,                // 5
            Email.DISPLAY_NAME_ALTERNATIVE,  // 6
        };
        ///M:[Sim Contact Flow] @{
        private static final String[] PROJECTION_ALTERNATIVE;
        static {
            List<String> projectionList = Lists.newArrayList(PROJECTION_ALTERNATIVE_INTERNAL);
            if (ContactsPortableUtils.MTK_PHONE_BOOK_SUPPORT) {
                projectionList.add(MtkContactsContract.RawContactsColumns.INDICATE_PHONE_SIM); //7
                projectionList.add(MtkContactsContract.RawContactsColumns.IS_SDN_CONTACT); //8
            }
            PROJECTION_ALTERNATIVE = projectionList.toArray(new String[projectionList.size()]);
        }
        /// @}

        public static final int EMAIL_ID           = 0;
        public static final int EMAIL_TYPE         = 1;
        public static final int EMAIL_LABEL        = 2;
        public static final int EMAIL_ADDRESS      = 3;
        public static final int EMAIL_PHOTO_ID     = 4;
        public static final int EMAIL_LOOKUP_KEY   = 5;
        public static final int EMAIL_DISPLAY_NAME = 6;
    }

    private final CharSequence mUnknownNameText;

    public EmailAddressListAdapter(Context context) {
        super(context);

        mUnknownNameText = context.getText(android.R.string.unknownName);
    }

    @Override
    public void configureLoader(CursorLoader loader, long directoryId) {
        Log.d(TAG,"[configureLoader]directoryId = " + directoryId);
        final Builder builder;
        if (isSearchMode()) {
            builder = Email.CONTENT_FILTER_URI.buildUpon();
            String query = getQueryString();
            builder.appendPath(TextUtils.isEmpty(query) ? "" : query);
        } else {
            builder = Email.CONTENT_URI.buildUpon();
            if (isSectionHeaderDisplayEnabled()) {
                builder.appendQueryParameter(Email.EXTRA_ADDRESS_BOOK_INDEX, "true");
            }
        }
        builder.appendQueryParameter(ContactsContract.DIRECTORY_PARAM_KEY,
                String.valueOf(directoryId));
        builder.appendQueryParameter(ContactsContract.REMOVE_DUPLICATE_ENTRIES, "true");
        loader.setUri(builder.build());

        if (getContactNameDisplayOrder() == ContactsPreferences.DISPLAY_ORDER_PRIMARY) {
            loader.setProjection(EmailQuery.PROJECTION_PRIMARY);
        } else {
            loader.setProjection(EmailQuery.PROJECTION_ALTERNATIVE);
        }

        if (getSortOrder() == ContactsPreferences.SORT_ORDER_PRIMARY) {
            loader.setSortOrder(Email.SORT_KEY_PRIMARY);
        } else {
            loader.setSortOrder(Email.SORT_KEY_ALTERNATIVE);
        }
    }

    @Override
    public String getContactDisplayName(int position) {
        return ((Cursor) getItem(position)).getString(EmailQuery.EMAIL_DISPLAY_NAME);
    }

    /**
     * Builds a {@link Data#CONTENT_URI} for the current cursor
     * position.
     */
    public Uri getDataUri(int position) {
        long id = ((Cursor) getItem(position)).getLong(EmailQuery.EMAIL_ID);
        return ContentUris.withAppendedId(Data.CONTENT_URI, id);
    }

    @Override
    protected ContactListItemView newView(
            Context context, int partition, Cursor cursor, int position, ViewGroup parent) {
        ContactListItemView view = super.newView(context, partition, cursor, position, parent);
        view.setUnknownNameText(mUnknownNameText);
        view.setQuickContactEnabled(isQuickContactEnabled());
        return view;
    }

    @Override
    protected void bindView(View itemView, int partition, Cursor cursor, int position) {
        Log.d(TAG,"[bindView]partition = " + partition + ",position = " + position);
        super.bindView(itemView, partition, cursor, position);
        ContactListItemView view = (ContactListItemView)itemView;
        bindSectionHeaderAndDivider(view, position);
        bindName(view, cursor);
        bindViewId(view, cursor, EmailQuery.EMAIL_ID);
        bindPhoto(view, cursor);
        bindEmailAddress(view, cursor);
    }

    protected void bindEmailAddress(ContactListItemView view, Cursor cursor) {
        CharSequence label = null;
        if (!cursor.isNull(EmailQuery.EMAIL_TYPE)) {
            final int type = cursor.getInt(EmailQuery.EMAIL_TYPE);
            final String customLabel = cursor.getString(EmailQuery.EMAIL_LABEL);

            // TODO cache
            label = Email.getTypeLabel(getContext().getResources(), type, customLabel);
        }
        view.setLabel(label);
        view.showData(cursor, EmailQuery.EMAIL_ADDRESS);
    }

    protected void bindSectionHeaderAndDivider(final ContactListItemView view, int position) {
        final int section = getSectionForPosition(position);
        if (getPositionForSection(section) == position) {
            String title = (String)getSections()[section];
            view.setSectionHeader(title);
        } else {
            view.setSectionHeader(null);
        }
    }

    protected void bindName(final ContactListItemView view, Cursor cursor) {
        view.showDisplayName(cursor, EmailQuery.EMAIL_DISPLAY_NAME, getContactNameDisplayOrder());
    }

    protected void bindPhoto(final ContactListItemView view, Cursor cursor) {
        long photoId = 0;
        if (!cursor.isNull(EmailQuery.EMAIL_PHOTO_ID)) {
            photoId = cursor.getLong(EmailQuery.EMAIL_PHOTO_ID);
        }
        DefaultImageRequest request = null;
        if (photoId == 0) {
             request = getDefaultImageRequestFromCursor(cursor, EmailQuery.EMAIL_DISPLAY_NAME,
                    EmailQuery.EMAIL_LOOKUP_KEY);
        }
        getPhotoLoader().loadThumbnail(view.getPhotoView(), photoId, false, getCircularPhotos(),
                request);
    }
//
//    protected void bindSearchSnippet(final ContactListItemView view, Cursor cursor) {
//        view.showSnippet(cursor, SUMMARY_SNIPPET_MIMETYPE_COLUMN_INDEX,
//                SUMMARY_SNIPPET_DATA1_COLUMN_INDEX, SUMMARY_SNIPPET_DATA4_COLUMN_INDEX);
//    }

}
