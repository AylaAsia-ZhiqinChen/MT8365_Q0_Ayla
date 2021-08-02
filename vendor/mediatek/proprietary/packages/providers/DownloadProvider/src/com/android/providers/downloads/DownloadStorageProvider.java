/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.providers.downloads;

import static com.android.providers.downloads.MediaStoreDownloadsHelper.getDocIdForMediaStoreDownload;
import static com.android.providers.downloads.MediaStoreDownloadsHelper.getMediaStoreIdString;
import static com.android.providers.downloads.MediaStoreDownloadsHelper.getMediaStoreUri;
import static com.android.providers.downloads.MediaStoreDownloadsHelper.isMediaStoreDownload;
import static com.android.providers.downloads.MediaStoreDownloadsHelper.isMediaStoreDownloadDir;
import static com.android.providers.downloads.Constants.DEBUG;

import android.annotation.NonNull;
import android.annotation.Nullable;
import android.app.DownloadManager;
import android.app.DownloadManager.Query;
import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.content.UriPermission;
import android.content.res.AssetFileDescriptor;
import android.database.Cursor;
import android.database.MatrixCursor;
import android.database.MatrixCursor.RowBuilder;
import android.graphics.Point;
import android.drm.DrmManagerClient;
import android.media.MediaFile;
import android.net.Uri;
import android.os.Binder;
import android.os.Bundle;
import android.os.CancellationSignal;
import android.os.Environment;
import android.os.FileObserver;
import android.os.FileUtils;
import android.os.ParcelFileDescriptor;
import android.provider.DocumentsContract;
import android.provider.DocumentsContract.Document;
import android.provider.DocumentsContract.Path;
import android.provider.DocumentsContract.Root;
import android.provider.Downloads;
import android.provider.MediaStore;
import android.provider.MediaStore.DownloadColumns;
import android.text.TextUtils;
import android.util.Log;
import android.util.Pair;

import com.android.internal.annotations.GuardedBy;
import com.android.internal.content.FileSystemProvider;
import com.mediatek.media.MtkMediaStore;
import com.mediatek.omadrm.OmaDrmStore;
import libcore.io.IoUtils;

import java.io.File;
import java.io.FileNotFoundException;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * Presents files located in {@link Environment#DIRECTORY_DOWNLOADS} and contents from
 * {@link DownloadManager}. {@link DownloadManager} contents include active downloads
 and completed
 * downloads added by other applications using
 * {@link DownloadManager#addCompletedDownload(String,
 String, boolean, String, String, long, boolean, boolean, Uri, Uri)}
 * .
 */
public class DownloadStorageProvider extends FileSystemProvider {
    private static final String TAG = "DownloadStorageProvider";
    private static final boolean DEBUG = false;

    private static final String AUTHORITY = Constants.STORAGE_AUTHORITY;
    private static final String DOC_ID_ROOT = Constants.STORAGE_ROOT_ID;
    private static final String XTAG_DRM = "DownloadManager/DRM";

    private static final String[] DEFAULT_ROOT_PROJECTION = new String[] {
            Root.COLUMN_ROOT_ID, Root.COLUMN_FLAGS, Root.COLUMN_ICON,
            Root.COLUMN_TITLE, Root.COLUMN_DOCUMENT_ID, Root.COLUMN_QUERY_ARGS
    };

    private static final String[] DEFAULT_DOCUMENT_PROJECTION = new String[] {
            Document.COLUMN_DOCUMENT_ID, Document.COLUMN_MIME_TYPE, Document.COLUMN_DISPLAY_NAME,
            Document.COLUMN_SUMMARY, Document.COLUMN_LAST_MODIFIED, Document.COLUMN_FLAGS,
            Document.COLUMN_SIZE,
            /// M: add to support drm .@{
            MediaStore.MediaColumns.DATA, MediaStore.MediaColumns.IS_DRM,
            MtkMediaStore.MediaColumns.DRM_METHOD,
            /// @}
    };

    private DownloadManager mDm;
    //private DocumentArchiveHelper mArchiveHelper;
    /// M : add to support drm. @{
    private static DrmManagerClient mDrmClient;
    /// @}


    private static final int NO_LIMIT = -1;

    @Override
    public boolean onCreate() {
        super.onCreate(DEFAULT_DOCUMENT_PROJECTION);
        mDm = (DownloadManager) getContext().getSystemService(Context.DOWNLOAD_SERVICE);
        mDm.setAccessAllDownloads(true);
        mDm.setAccessFilename(true);

        return true;
    }

    private static String[] resolveRootProjection(String[] projection) {
        return projection != null ? projection : DEFAULT_ROOT_PROJECTION;
    }

    private static String[] resolveDocumentProjection(String[] projection) {
        return projection != null ? projection : DEFAULT_DOCUMENT_PROJECTION;
    }

    private void copyNotificationUri(MatrixCursor result, Cursor cursor) {
        result.setNotificationUri(getContext().getContentResolver(), cursor.getNotificationUri());
    }

    /**
     * Called by {@link DownloadProvider} when deleting a row in the {@link DownloadManager}
     * database.
     */
    static void onDownloadProviderDelete(Context context, long id) {
        final Uri uri = DocumentsContract.buildDocumentUri(AUTHORITY, Long.toString(id));
        context.revokeUriPermission(uri, ~0);
    }

    static void onMediaProviderDownloadsDelete(Context context, long[] ids, String[] mimeTypes) {
        for (int i = 0; i < ids.length; ++i) {
            final boolean isDir = mimeTypes[i] == null;
            final Uri uri = DocumentsContract.buildDocumentUri(AUTHORITY,
                    MediaStoreDownloadsHelper.getDocIdForMediaStoreDownload(ids[i], isDir));
            context.revokeUriPermission(uri, ~0);
        }
    }

    static void revokeAllMediaStoreUriPermissions(Context context) {
        final List<UriPermission> uriPermissions =
                context.getContentResolver().getOutgoingUriPermissions();
        final int size = uriPermissions.size();
        final StringBuilder sb = new StringBuilder("Revoking permissions for uris: ");
        for (int i = 0; i < size; ++i) {
            final Uri uri = uriPermissions.get(i).getUri();
            if (AUTHORITY.equals(uri.getAuthority())
                    && isMediaStoreDownload(DocumentsContract.getDocumentId(uri))) {
                context.revokeUriPermission(uri, ~0);
                sb.append(uri + ",");
            }
        }
        Log.d(TAG, sb.toString());
    }

    @Override
    public Cursor queryRoots(String[] projection) throws FileNotFoundException {
        // It's possible that the folder does not exist on disk, so we will create the folder if
        // that is the case. If user decides to delete the folder later, then it's OK to fail on
        // subsequent queries.
        getPublicDownloadsDirectory().mkdirs();

        final MatrixCursor result = new MatrixCursor(resolveRootProjection(projection));
        final RowBuilder row = result.newRow();
        row.add(Root.COLUMN_ROOT_ID, DOC_ID_ROOT);
        row.add(Root.COLUMN_FLAGS, Root.FLAG_LOCAL_ONLY | Root.FLAG_SUPPORTS_RECENTS
                | Root.FLAG_SUPPORTS_CREATE | Root.FLAG_SUPPORTS_SEARCH
                | Root.FLAG_SUPPORTS_IS_CHILD);
        row.add(Root.COLUMN_ICON, R.mipmap.ic_launcher_download);
        row.add(Root.COLUMN_TITLE, getContext().getString(R.string.root_downloads));
        row.add(Root.COLUMN_DOCUMENT_ID, DOC_ID_ROOT);
        row.add(Root.COLUMN_QUERY_ARGS, SUPPORTED_QUERY_ARGS);
        return result;
    }

    @Override
    public Path findDocumentPath(@Nullable String parentDocId, String docId) throws FileNotFoundException {

        // parentDocId is null if the client is asking for the path to the root of a doc tree.
        // Don't share root information with those who shouldn't know it.
        final String rootId = (parentDocId == null) ? DOC_ID_ROOT : null;

        if (parentDocId == null) {
            parentDocId = DOC_ID_ROOT;
        }

        final File parent = getFileForDocId(parentDocId);

        final File doc = getFileForDocId(docId);

        return new Path(rootId, findDocumentPath(parent, doc));
    }

    /**
     * Calls on {@link FileSystemProvider#createDocument(String, String, String)}, and then creates
     * a new database entry in {@link DownloadManager} if it is not a raw file and not a folder.
     */
    @Override
    public String createDocument(String parentDocId, String mimeType, String displayName)
            throws FileNotFoundException {
        if (DEBUG) Log.d(Constants.DL_ENHANCE,
            "DownloadStorageProvider:createDocument(), docId: " + parentDocId +
                       ", mimeType: " + mimeType + ",displayName: " + displayName);
        // Delegate to real provider
        final long token = Binder.clearCallingIdentity();
        try {
            String newDocumentId = super.createDocument(parentDocId, mimeType, displayName);
            if (!Document.MIME_TYPE_DIR.equals(mimeType)
                    && !RawDocumentsHelper.isRawDocId(parentDocId)
                    && !isMediaStoreDownload(parentDocId)) {
                File newFile = getFileForDocId(newDocumentId);
                newDocumentId = Long.toString(mDm.addCompletedDownload(
                        newFile.getName(), newFile.getName(), true, mimeType,
                        newFile.getAbsolutePath(), 0L,
                        false, true));
            }
            return newDocumentId;
        } finally {
            Binder.restoreCallingIdentity(token);
        }
    }

    @Override
    public void deleteDocument(String docId) throws FileNotFoundException {
        // Delegate to real provider
        final long token = Binder.clearCallingIdentity();
        try {
            if (RawDocumentsHelper.isRawDocId(docId) || isMediaStoreDownload(docId)) {
                super.deleteDocument(docId);
                return;
            }

            if (mDm.remove(Long.parseLong(docId)) != 1) {
                throw new IllegalStateException("Failed to delete " + docId);
            }
        } finally {
            Binder.restoreCallingIdentity(token);
        }
    }

    @Override
    public String renameDocument(String docId, String displayName)
            throws FileNotFoundException {
        final long token = Binder.clearCallingIdentity();

        try {
            if (RawDocumentsHelper.isRawDocId(docId)
                    || isMediaStoreDownloadDir(docId)) {
                return super.renameDocument(docId, displayName);
            }

            displayName = FileUtils.buildValidFatFilename(displayName);
            if (isMediaStoreDownload(docId)) {
                renameMediaStoreDownload(docId, displayName);
            } else {
            final long id = Long.parseLong(docId);
            if (!mDm.rename(getContext(), id, displayName)) {
                throw new IllegalStateException(
                        "Failed to rename to " + displayName + " in downloadsManager");
            }
            }
            return null;
        } finally {
            Binder.restoreCallingIdentity(token);
        }
    }

    @Override
    public Cursor queryDocument(String docId, String[] projection) throws FileNotFoundException {
        // Delegate to real provider
        final long token = Binder.clearCallingIdentity();
        Cursor cursor = null;
        try {
            if (RawDocumentsHelper.isRawDocId(docId)) {
                return super.queryDocument(docId, projection);
            }

            final DownloadsCursor result = new DownloadsCursor(projection,
                    getContext().getContentResolver());

            if (DOC_ID_ROOT.equals(docId)) {
                includeDefaultDocument(result);
            } else if (isMediaStoreDownload(docId)) {
                cursor = getContext().getContentResolver().query(getMediaStoreUri(docId),
                        null, null, null);
                copyNotificationUri(result, cursor);
                if (cursor.moveToFirst()) {
                    includeDownloadFromMediaStore(result, cursor, null /* filePaths */);
                }
            } else {
                cursor = mDm.query(new Query().setFilterById(Long.parseLong(docId)));
                copyNotificationUri(result, cursor);
                Set<String> filePaths = new HashSet<>();
                if (cursor.moveToFirst()) {
                    // We don't know if this queryDocument() call is from Downloads (manage)
                    // or Files. Safely assume it's Files.
                    includeDownloadFromCursor(result, cursor, null /* filePaths */,
                            null /* queryArgs */);
                }
            }
            result.start();
            return result;
        } finally {
            IoUtils.closeQuietly(cursor);
            Binder.restoreCallingIdentity(token);
        }
    }

    @Override
    public Cursor queryChildDocuments(String parentDocId, String[] projection, String sortOrder)
            throws FileNotFoundException {
        return queryChildDocuments(parentDocId, projection, sortOrder, false);
    }

    @Override
    public Cursor queryChildDocumentsForManage(
            String parentDocId, String[] projection, String sortOrder)
            throws FileNotFoundException {
        return queryChildDocuments(parentDocId, projection, sortOrder, true);
    }

    private Cursor queryChildDocuments(String parentDocId, String[] projection,
            String sortOrder, boolean manage) throws FileNotFoundException {

        // Delegate to real provider
        final long token = Binder.clearCallingIdentity();
        Cursor cursor = null;
        try {
            if (RawDocumentsHelper.isRawDocId(parentDocId)) {
                return super.queryChildDocuments(parentDocId, projection, sortOrder);
            }

            Log.d(Constants.DL_ENHANCE,
                            "Anuj projection = " + projection);
            final DownloadsCursor result = new DownloadsCursor(projection,
                    getContext().getContentResolver());
            final ArrayList<Uri> notificationUris = new ArrayList<>();
            if (isMediaStoreDownloadDir(parentDocId)) {
                includeDownloadsFromMediaStore(result, null /* queryArgs */,
                        null /* filePaths */, notificationUris,
                        getMediaStoreIdString(parentDocId), NO_LIMIT, manage);
            } else {
                assert (DOC_ID_ROOT.equals(parentDocId));
                if (manage) {
                    cursor = mDm.query(
                            new DownloadManager.Query().setOnlyIncludeVisibleInDownloadsUi(true));
                } else {
                    cursor = mDm.query(
                            new DownloadManager.Query().setOnlyIncludeVisibleInDownloadsUi(true)
                                    .setFilterByStatus(DownloadManager.STATUS_SUCCESSFUL));
                }
                final Set<String> filePaths = new HashSet<>();
                while (cursor.moveToNext()) {
                    includeDownloadFromCursor(result, cursor, filePaths, null /* queryArgs */);
                }
                notificationUris.add(cursor.getNotificationUri());
                includeDownloadsFromMediaStore(result, null /* queryArgs */,
                        filePaths, notificationUris,
                        null /* parentId */, NO_LIMIT, manage);
                includeFilesFromSharedStorage(result, filePaths, null);
            }
            result.setNotificationUris(getContext().getContentResolver(), notificationUris);
            result.start();
            return result;
        } finally {
            IoUtils.closeQuietly(cursor);
            Binder.restoreCallingIdentity(token);
        }
    }

    @Override
    public Cursor queryRecentDocuments(String rootId, String[] projection,
            @Nullable Bundle queryArgs, @Nullable CancellationSignal signal)
            throws FileNotFoundException {
        final DownloadsCursor result =
                new DownloadsCursor(projection, getContext().getContentResolver());

        // Delegate to real provider
        final long token = Binder.clearCallingIdentity();

        int limit = 12;
        if (queryArgs != null) {
            limit = queryArgs.getInt(ContentResolver.QUERY_ARG_LIMIT, -1);

            if (limit < 0) {
                // Use default value, and no QUERY_ARG* is honored.
                limit = 12;
            } else {
                // We are honoring the QUERY_ARG_LIMIT.
                Bundle extras = new Bundle();
                result.setExtras(extras);
                extras.putStringArray(ContentResolver.EXTRA_HONORED_ARGS, new String[]{
                        ContentResolver.QUERY_ARG_LIMIT
                });
            }
        }

        Cursor cursor = null;
        final ArrayList<Uri> notificationUris = new ArrayList<>();
        try {
            cursor = mDm.query(new DownloadManager.Query().setOnlyIncludeVisibleInDownloadsUi(true)
                    .setFilterByStatus(DownloadManager.STATUS_SUCCESSFUL));
            final Set<String> filePaths = new HashSet<>();
            while (cursor.moveToNext() && result.getCount() < limit) {
                final String mimeType = cursor.getString(
                        cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_MEDIA_TYPE));
                final String uri = cursor.getString(
                        cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_MEDIAPROVIDER_URI));

                // Skip images and videos that have been inserted into the MediaStore so we
                // don't duplicate them in the recent list. The audio root of
                // MediaDocumentsProvider doesn't support recent, we add it into recent list.
                if (mimeType == null || (MediaFile.isImageMimeType(mimeType)
                        || MediaFile.isVideoMimeType(mimeType)) && !TextUtils.isEmpty(uri)) {
                    continue;
                }
                includeDownloadFromCursor(result, cursor, filePaths,
                        null /* queryArgs */);
            }
            notificationUris.add(cursor.getNotificationUri());

            // Skip media files that have been inserted into the MediaStore so we
            // don't duplicate them in the recent list.
            final Bundle args = new Bundle();
            args.putBoolean(DocumentsContract.QUERY_ARG_EXCLUDE_MEDIA, true);

            includeDownloadsFromMediaStore(result, args, filePaths,
                    notificationUris, null /* parentId */, (limit - result.getCount()),
                    false /* includePending */);
        } finally {
            IoUtils.closeQuietly(cursor);
            Binder.restoreCallingIdentity(token);
        }

        result.setNotificationUris(getContext().getContentResolver(), notificationUris);
        result.start();
        return result;
    }

    @Override
    public Cursor querySearchDocuments(String rootId, String[] projection, Bundle queryArgs)
            throws FileNotFoundException {

        final DownloadsCursor result =
                new DownloadsCursor(projection, getContext().getContentResolver());
        final ArrayList<Uri> notificationUris = new ArrayList<>();

        // Delegate to real provider
        final long token = Binder.clearCallingIdentity();
        Cursor cursor = null;
        try {
            cursor = mDm.query(new DownloadManager.Query().setOnlyIncludeVisibleInDownloadsUi(true)
                    .setFilterByString(DocumentsContract.getSearchDocumentsQuery(queryArgs)));
            final Set<String> filePaths = new HashSet<>();
            while (cursor.moveToNext()) {
                includeDownloadFromCursor(result, cursor, filePaths, queryArgs);
            }
            notificationUris.add(cursor.getNotificationUri());
            includeDownloadsFromMediaStore(result, queryArgs, filePaths,
                    notificationUris, null /* parentId */, NO_LIMIT, true /* includePending */);

            includeSearchFilesFromSharedStorage(result, projection, filePaths, queryArgs);
        } finally {
            IoUtils.closeQuietly(cursor);
            Binder.restoreCallingIdentity(token);
        }

        final String[] handledQueryArgs = DocumentsContract.getHandledQueryArguments(queryArgs);
        if (handledQueryArgs.length > 0) {
            final Bundle extras = new Bundle();
            extras.putStringArray(ContentResolver.EXTRA_HONORED_ARGS, handledQueryArgs);
            result.setExtras(extras);
        }

        result.setNotificationUris(getContext().getContentResolver(), notificationUris);
        result.start();
        return result;
    }

    private void includeSearchFilesFromSharedStorage(DownloadsCursor result,
            String[] projection, Set<String> filePaths,
            Bundle queryArgs) throws FileNotFoundException {
        final File downloadDir = getPublicDownloadsDirectory();
            try (Cursor rawFilesCursor = super.querySearchDocuments(downloadDir,
                    projection, filePaths, queryArgs)) {

                final boolean shouldExcludeMedia = queryArgs.getBoolean(
                        DocumentsContract.QUERY_ARG_EXCLUDE_MEDIA, false /* defaultValue */);
                while (rawFilesCursor.moveToNext()) {
                    final String mimeType = rawFilesCursor.getString(
                            rawFilesCursor.getColumnIndexOrThrow(Document.COLUMN_MIME_TYPE));
                    // When the value of shouldExcludeMedia is true, don't add media files into
                    // the result to avoid duplicated files. MediaScanner will scan the files
                    // into MediaStore. If the behavior is changed, we need to add the files back.
                    if (!shouldExcludeMedia || !isMediaMimeType(mimeType)) {
                        String docId = rawFilesCursor.getString(
                                rawFilesCursor.getColumnIndexOrThrow(Document.COLUMN_DOCUMENT_ID));
                        File rawFile = getFileForDocId(docId);
                        includeFileFromSharedStorage(result, rawFile);
                    }
                }
            }
    }

    @Override
    public String getDocumentType(String docId) throws FileNotFoundException {
        // Delegate to real provider
        final long token = Binder.clearCallingIdentity();
        try {
            if (RawDocumentsHelper.isRawDocId(docId)) {
                return super.getDocumentType(docId);
            }

            final ContentResolver resolver = getContext().getContentResolver();
            final Uri contentUri;
            if (isMediaStoreDownload(docId)) {
                contentUri = getMediaStoreUri(docId);
            } else {
                final long id = Long.parseLong(docId);
                contentUri = mDm.getDownloadUri(id);
            }
            return resolver.getType(contentUri);
        } finally {
            Binder.restoreCallingIdentity(token);
        }
    }

    @Override
    public ParcelFileDescriptor openDocument(String docId, String mode, CancellationSignal signal)
            throws FileNotFoundException {
        // Delegate to real provider
        final long token = Binder.clearCallingIdentity();
        try {
            if (RawDocumentsHelper.isRawDocId(docId)) {
                return super.openDocument(docId, mode, signal);
            }

            final ContentResolver resolver = getContext().getContentResolver();
            final Uri contentUri;
            if (isMediaStoreDownload(docId)) {
                contentUri = getMediaStoreUri(docId);
            } else {
                final long id = Long.parseLong(docId);
                contentUri = mDm.getDownloadUri(id);
            }
            return resolver.openFileDescriptor(contentUri, mode, signal);
        } finally {
            Binder.restoreCallingIdentity(token);
        }
    }

    @Override
    public AssetFileDescriptor openDocumentThumbnail(
            String docId, Point sizeHint, CancellationSignal signal) throws FileNotFoundException {
        // TODO: extend ExifInterface to support fds
        final ParcelFileDescriptor pfd = openDocument(docId, "r", signal);
        return new AssetFileDescriptor(pfd, 0, AssetFileDescriptor.UNKNOWN_LENGTH);
    }

    @Override
    protected File getFileForDocId(String docId, boolean visible) throws FileNotFoundException {
        if (RawDocumentsHelper.isRawDocId(docId)) {
            return new File(RawDocumentsHelper.getAbsoluteFilePath(docId));
        }

        if (isMediaStoreDownload(docId)) {
            return getFileForMediaStoreDownload(docId);
        }
        if (DOC_ID_ROOT.equals(docId)) {
            return getPublicDownloadsDirectory();
        }

        final long token = Binder.clearCallingIdentity();
        Cursor cursor = null;
        String localFilePath = null;
        try {
            cursor = mDm.query(new Query().setFilterById(Long.parseLong(docId)));
            if (cursor.moveToFirst()) {
                localFilePath = cursor.getString(
                        cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_LOCAL_FILENAME));
            }
        } finally {
            IoUtils.closeQuietly(cursor);
            Binder.restoreCallingIdentity(token);
        }

        if (localFilePath == null) {
            throw new IllegalStateException("File has no filepath. Could not be found.");
        }
        return new File(localFilePath);
    }

    @Override
    protected String getDocIdForFile(File file) throws FileNotFoundException {
        return RawDocumentsHelper.getDocIdForFile(file);
    }

    @Override
    protected Uri buildNotificationUri(String docId) {
        return DocumentsContract.buildChildDocumentsUri(AUTHORITY, docId);
    }

    private static boolean isMediaMimeType(String mimeType) {
        return MediaFile.isImageMimeType(mimeType) || MediaFile.isVideoMimeType(mimeType)
                || MediaFile.isAudioMimeType(mimeType);
    }

    private void includeDefaultDocument(MatrixCursor result) {
        final RowBuilder row = result.newRow();
        row.add(Document.COLUMN_DOCUMENT_ID, DOC_ID_ROOT);
        // We have the same display name as our root :)
        row.add(Document.COLUMN_DISPLAY_NAME,
                getContext().getString(R.string.root_downloads));
        row.add(Document.COLUMN_MIME_TYPE, Document.MIME_TYPE_DIR);
        row.add(Document.COLUMN_FLAGS,
                Document.FLAG_DIR_PREFERS_LAST_MODIFIED | Document.FLAG_DIR_SUPPORTS_CREATE);
    }

    /**
     * Adds the entry from the cursor to the result only if the entry is valid. That is,
     * if the file exists in the file system.
     */
    private void includeDownloadFromCursor(MatrixCursor result, Cursor cursor,
            Set<String> filePaths, Bundle queryArgs) {
        final long id = cursor.getLong(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_ID));
        final String docId = String.valueOf(id);

        final String displayName = cursor.getString(
                cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_TITLE));
        String summary = cursor.getString(
                cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_DESCRIPTION));
        String mimeType = cursor.getString(
                cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_MEDIA_TYPE));
        if (mimeType == null) {
            // Provide fake MIME type so it's openable
            mimeType = "vnd.android.document/file";
        }

        if (queryArgs != null) {
            final boolean shouldExcludeMedia = queryArgs.getBoolean(
                    DocumentsContract.QUERY_ARG_EXCLUDE_MEDIA, false /* defaultValue */);
            if (shouldExcludeMedia) {
                final String uri = cursor.getString(
                        cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_MEDIAPROVIDER_URI));

                // Skip media files that have been inserted into the MediaStore so we
                // don't duplicate them in the search list.
                if (isMediaMimeType(mimeType) && !TextUtils.isEmpty(uri)) {
                    return;
                }
            }
        }

        // size could be -1 which indicates that download hasn't started.
        final long size = cursor.getLong(
                cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_TOTAL_SIZE_BYTES));

        String localFilePath = cursor.getString(
                cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_LOCAL_FILENAME));

        int extraFlags = Document.FLAG_PARTIAL;
        final int status = cursor.getInt(
                cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_STATUS));
        switch (status) {
            case DownloadManager.STATUS_SUCCESSFUL:
                // Verify that the document still exists in external storage. This is necessary
                // because files can be deleted from the file system without their entry being
                // removed from DownloadsManager.
                if (localFilePath == null || !new File(localFilePath).exists()) {
                    return;
                }
                extraFlags = Document.FLAG_SUPPORTS_RENAME;  // only successful is non-partial
                break;
            case DownloadManager.STATUS_PAUSED:
                summary = getContext().getString(R.string.download_queued);
                break;
            case DownloadManager.STATUS_PENDING:
                summary = getContext().getString(R.string.download_queued);
                break;
            case DownloadManager.STATUS_RUNNING:
                final long progress = cursor.getLong(cursor.getColumnIndexOrThrow(
                        DownloadManager.COLUMN_BYTES_DOWNLOADED_SO_FAR));
                if (size > 0) {
                    String percent =
                            NumberFormat.getPercentInstance().format((double) progress / size);
                    summary = getContext().getString(R.string.download_running_percent, percent);
                } else {
                    summary = getContext().getString(R.string.download_running);
                }
                break;
            case DownloadManager.STATUS_FAILED:
                /// M : add OMA DL error string. @{
                summary = getContext().getString(R.string.download_error);
                if (MtkDownloads.Impl.OMA_DOWNLOAD_SUPPORT) {
                     int reasonColumnId =
                      cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_REASON);
                     int failedReason = cursor.getInt(reasonColumnId);
                     if (failedReason == DownloadManager.ERROR_INSUFFICIENT_SPACE) {
                         summary = getContext().
                         getString(R.string.download_error_insufficient_memory);
                     } else if (failedReason == Downloads.Impl.STATUS_BAD_REQUEST) {
                         summary = getContext().
                         getString(R.string.download_error_invalid_descriptor);
                     } else if (failedReason ==
                      MtkDownloads.Impl.OMADL_STATUS_ERROR_INVALID_DDVERSION) {
                         summary = getContext().
                         getString(R.string.download_error_invalid_ddversion);
                     } else if (failedReason ==
                      MtkDownloads.Impl.OMADL_STATUS_ERROR_ATTRIBUTE_MISMATCH) {
                         summary = getContext().
                         getString(R.string.download_error_attribute_mismatch);
                     }
                }
                break;
                /// @}
            default:
                summary = getContext().getString(R.string.download_error);
                break;
        }

        /// M: get origin mimetype of drm file. @{
        int isDrm = 0;
        int drmMethod = 0;

        if (Helpers.isMtkDRMFile(mimeType)) {
            isDrm = 1;
        }

        //Anuj
        if (null != mimeType) {
             if (mimeType.equals(OmaDrmStore.DrmObjectMimeType.MIME_TYPE_DRM_CONTENT) ||
                     mimeType.equals(OmaDrmStore.DrmObjectMimeType.MIME_TYPE_DRM_MESSAGE)) {
                 String localUri = cursor.getString(cursor.getColumnIndexOrThrow(
                                 DownloadManager.COLUMN_LOCAL_URI));
                 if (localUri != null) {
                     if (mDrmClient == null) {
                         mDrmClient = new DrmManagerClient(getContext());
                     }

                     String path = Uri.parse(localUri).getPath();
                     String oriMimeType = mDrmClient.getOriginalMimeType(path);
                     drmMethod = getMethod(mDrmClient, path);
                     if (null != oriMimeType && !oriMimeType.isEmpty()) {
                         mimeType = oriMimeType;
                         if (DEBUG) Log.d(XTAG_DRM,
                            "DownloadStorageProvider: includeDownloadFromCursor a DRM file path:"
                                 + path + " ,original MimeType is:" + mimeType +
                                 ", docId: " + docId);
                     }
                 }
             }
        }

        final long lastModified = cursor.getLong(
                cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_LAST_MODIFIED_TIMESTAMP));

        if (!DocumentsContract.matchSearchQueryArguments(queryArgs, displayName, mimeType,
                lastModified, size)) {
            return;
        }

        includeDownload(result, docId, displayName, summary, size, mimeType,
                lastModified, extraFlags, status == DownloadManager.STATUS_RUNNING);
        /// M: add to support drm. @{
        //ANuj
        if(Constants.MTK_DRM_ENABLED) {
        if (isDrm == 1) {
            String localPath = cursor
                    .getString(cursor
                            .getColumnIndexOrThrow(DownloadManager.COLUMN_LOCAL_FILENAME));

            if (localPath != null && localPath.length() != 0) {
                Uri mediaFilesUri = MediaStore.Files.getContentUri("external");
                String whereClause = MediaStore.MediaColumns.DATA + " = ?";

                Cursor drmCursor = null;
                try {
                    drmCursor = getContext().getContentResolver().query(
                            mediaFilesUri,
                            new String[] { MediaStore.MediaColumns.IS_DRM},
                            whereClause, new String[] { localPath }, null);

                    if (drmCursor != null && drmCursor.moveToFirst()) {
                        isDrm = drmCursor
                                .getInt(drmCursor
                                        .getColumnIndex(MediaStore.MediaColumns.IS_DRM));
                    }
                } catch (IllegalStateException e) {
                    Log.e(Constants.DL_ENHANCE,
                     "DownloadStorageProvider:includeDownloadFromCursor query media occur error");
                } finally {
                    if (drmCursor != null) {
                        drmCursor.close();
                    }
                }
                if (!new File(localPath).exists()) {
                    isDrm = 1;
                    drmMethod = -1;
                }
            }

        if (drmMethod == 0) {
                isDrm = 0;
            }
            final RowBuilder row = result.newRow();
            row.add(MediaStore.MediaColumns.DATA, localPath);
            row.add(MediaStore.MediaColumns.IS_DRM, isDrm);
            row.add(MtkMediaStore.MediaColumns.DRM_METHOD, drmMethod);
            Log.e(Constants.DL_ENHANCE, "localPath: " + localPath
                    + ", isDrm: " + isDrm + ", drmMethod: "
                    + drmMethod + ", mimeType: " + mimeType + ", docId: " + docId);
        }
        }
        if (filePaths != null && localFilePath != null) {
            filePaths.add(localFilePath);
        }
    }

    private void includeDownload(MatrixCursor result,
            String docId, String displayName, String summary, long size,
            String mimeType, long lastModifiedMs, int extraFlags, boolean isPending) {

        int flags = Document.FLAG_SUPPORTS_DELETE | Document.FLAG_SUPPORTS_WRITE | extraFlags;
        if (mimeType.startsWith("image/")) {
            flags |= Document.FLAG_SUPPORTS_THUMBNAIL;
        }

        if (typeSupportsMetadata(mimeType)) {
            flags |= Document.FLAG_SUPPORTS_METADATA;
        }

        final RowBuilder row = result.newRow();
        row.add(Document.COLUMN_DOCUMENT_ID, docId);
        row.add(Document.COLUMN_DISPLAY_NAME, displayName);
        row.add(Document.COLUMN_SUMMARY, summary);
        row.add(Document.COLUMN_SIZE, size == -1 ? null : size);
        row.add(Document.COLUMN_MIME_TYPE, mimeType);
        row.add(Document.COLUMN_FLAGS, flags);
        // Incomplete downloads get a null timestamp.  This prevents thrashy UI when a bunch of
        // active downloads get sorted by mod time.
        if (!isPending) {
            row.add(Document.COLUMN_LAST_MODIFIED, lastModifiedMs);
        }
    }

    /**
     * Takes all the top-level files from the Downloads directory and adds them to the result.
     *
     * @param result cursor containing all documents to be returned by queryChildDocuments or
     *            queryChildDocumentsForManage.
     * @param downloadedFilePaths The absolute file paths of all the files in the result Cursor.
     * @param searchString query used to filter out unwanted results.
     */
    private void includeFilesFromSharedStorage(DownloadsCursor result,
            Set<String> downloadedFilePaths, @Nullable String searchString)
            throws FileNotFoundException {
        final File downloadsDir = getPublicDownloadsDirectory();
        // Add every file from the Downloads directory to the result cursor. Ignore files that
        // were in the supplied downloaded file paths.
            for (File file : FileUtils.listFilesOrEmpty(downloadsDir)) {
                boolean inResultsAlready = downloadedFilePaths.contains(file.getAbsolutePath());
                boolean containsQuery = searchString == null || file.getName().contains(
                        searchString);
                if (!inResultsAlready && containsQuery) {
                    includeFileFromSharedStorage(result, file);
                }
            }
    }

    /**
     * Adds a file to the result cursor. It uses a combination of {@code #RAW_PREFIX} and its
     * absolute file path for its id. Directories are not to be included.
     *
     * @param result cursor containing all documents to be returned by queryChildDocuments or
     *            queryChildDocumentsForManage.
     * @param file file to be included in the result cursor.
     */
    private void includeFileFromSharedStorage(MatrixCursor result, File file)
            throws FileNotFoundException {
        includeFile(result, null, file);
    }

    private static File getPublicDownloadsDirectory() {
        return Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
    }

    private void renameMediaStoreDownload(String docId, String displayName) {
        final File before = getFileForMediaStoreDownload(docId);
        final File after = new File(before.getParentFile(), displayName);

        if (after.exists()) {
            throw new IllegalStateException("Already exists " + after);
        }
        if (!before.renameTo(after)) {
            throw new IllegalStateException("Failed to rename from " + before + " to " + after);
        }

        final long token = Binder.clearCallingIdentity();
        try {
            final Uri mediaStoreUri = getMediaStoreUri(docId);
            final ContentValues values = new ContentValues();
            values.put(DownloadColumns.DATA, after.getAbsolutePath());
            values.put(DownloadColumns.DISPLAY_NAME, displayName);
            final int count = getContext().getContentResolver().update(mediaStoreUri, values,
                    null, null);
            if (count != 1) {
                throw new IllegalStateException("Failed to update " + mediaStoreUri
                        + ", values=" + values);
            }
        } finally {
            Binder.restoreCallingIdentity(token);
        }
    }

    private File getFileForMediaStoreDownload(String docId) {
        final Uri mediaStoreUri = getMediaStoreUri(docId);
        final long token = Binder.clearCallingIdentity();
        try (Cursor cursor = queryForSingleItem(mediaStoreUri,
                new String[] { DownloadColumns.DATA }, null, null, null)) {
            final String filePath = cursor.getString(0);
            if (filePath == null) {
                throw new IllegalStateException("Missing _data for " + mediaStoreUri);
            }
            return new File(filePath);
        } catch (FileNotFoundException e) {
            throw new IllegalStateException(e);
        } finally {
            Binder.restoreCallingIdentity(token);
        }
    }

    private Pair<String, String> getRelativePathAndDisplayNameForDownload(long id) {
        final Uri mediaStoreUri = ContentUris.withAppendedId(
                MediaStore.Downloads.EXTERNAL_CONTENT_URI, id);
        final long token = Binder.clearCallingIdentity();
        try (Cursor cursor = queryForSingleItem(mediaStoreUri,
                new String[] { DownloadColumns.RELATIVE_PATH, DownloadColumns.DISPLAY_NAME },
                null, null, null)) {
            final String relativePath = cursor.getString(0);
            final String displayName = cursor.getString(1);
            if (relativePath == null || displayName == null) {
                throw new IllegalStateException(
                        "relative_path and _display_name should not be null for " + mediaStoreUri);
            }
            return Pair.create(relativePath, displayName);
        } catch (FileNotFoundException e) {
            throw new IllegalStateException(e);
        } finally {
            Binder.restoreCallingIdentity(token);
        }
    }

    /**
     * Copied from MediaProvider.java
     *
     * Query the given {@link Uri}, expecting only a single item to be found.
     *
     * @throws FileNotFoundException if no items were found, or multiple items
     *             were found, or there was trouble reading the data.
     */
    private Cursor queryForSingleItem(Uri uri, String[] projection,
            String selection, String[] selectionArgs, CancellationSignal signal)
            throws FileNotFoundException {
        final Cursor c = getContext().getContentResolver().query(uri, projection,
                ContentResolver.createSqlQueryBundle(selection, selectionArgs, null), signal);
        if (c == null) {
            throw new FileNotFoundException("Missing cursor for " + uri);
        } else if (c.getCount() < 1) {
            IoUtils.closeQuietly(c);
            throw new FileNotFoundException("No item at " + uri);
        } else if (c.getCount() > 1) {
            IoUtils.closeQuietly(c);
            throw new FileNotFoundException("Multiple items at " + uri);
        }

        if (c.moveToFirst()) {
            return c;
        } else {
            IoUtils.closeQuietly(c);
            throw new FileNotFoundException("Failed to read row from " + uri);
        }
    }


    private void includeDownloadsFromMediaStore(@NonNull MatrixCursor result,
            @Nullable Bundle queryArgs,
            @Nullable Set<String> filePaths, @NonNull ArrayList<Uri> notificationUris,
            @Nullable String parentId, int limit, boolean includePending) {
        if (limit == 0) {
            return;
        }

        final long token = Binder.clearCallingIdentity();
        final Pair<String, String[]> selectionPair
                = buildSearchSelection(queryArgs, filePaths, parentId);
        final Uri.Builder queryUriBuilder = MediaStore.Downloads.EXTERNAL_CONTENT_URI.buildUpon();
        if (limit != NO_LIMIT) {
            queryUriBuilder.appendQueryParameter(MediaStore.PARAM_LIMIT, String.valueOf(limit));
        }
        if (includePending) {
            MediaStore.setIncludePending(queryUriBuilder);
        }
        try (Cursor cursor = getContext().getContentResolver().query(
                queryUriBuilder.build(), null,
                selectionPair.first, selectionPair.second, null)) {
            while (cursor.moveToNext()) {
                includeDownloadFromMediaStore(result, cursor, filePaths);
            }
            notificationUris.add(MediaStore.Files.EXTERNAL_CONTENT_URI);
            notificationUris.add(MediaStore.Downloads.EXTERNAL_CONTENT_URI);
        } finally {
            Binder.restoreCallingIdentity(token);
        }
    }

    private void includeDownloadFromMediaStore(@NonNull MatrixCursor result,
            @NonNull Cursor mediaCursor, @Nullable Set<String> filePaths) {
        final String mimeType = getMimeType(mediaCursor);
        final boolean isDir = Document.MIME_TYPE_DIR.equals(mimeType);
        final String docId = getDocIdForMediaStoreDownload(
                mediaCursor.getLong(mediaCursor.getColumnIndex(DownloadColumns._ID)), isDir);
        final String displayName = mediaCursor.getString(
                mediaCursor.getColumnIndex(DownloadColumns.DISPLAY_NAME));
        final long size = mediaCursor.getLong(
                mediaCursor.getColumnIndex(DownloadColumns.SIZE));
        final long lastModifiedMs = mediaCursor.getLong(
                mediaCursor.getColumnIndex(DownloadColumns.DATE_MODIFIED)) * 1000;
        final boolean isPending = mediaCursor.getInt(
                mediaCursor.getColumnIndex(DownloadColumns.IS_PENDING)) == 1;

        int extraFlags = isPending ? Document.FLAG_PARTIAL : 0;
        if (Document.MIME_TYPE_DIR.equals(mimeType)) {
            extraFlags |= Document.FLAG_DIR_SUPPORTS_CREATE;
        }
        if (!isPending) {
            extraFlags |= Document.FLAG_SUPPORTS_RENAME;
        }

        includeDownload(result, docId, displayName, null /* description */, size, mimeType,
                lastModifiedMs, extraFlags, isPending);
        if (filePaths != null) {
            filePaths.add(mediaCursor.getString(
                    mediaCursor.getColumnIndex(DownloadColumns.DATA)));
        }
    }

    private String getMimeType(@NonNull Cursor mediaCursor) {
        final String mimeType = mediaCursor.getString(
                mediaCursor.getColumnIndex(DownloadColumns.MIME_TYPE));
        if (mimeType == null) {
            return Document.MIME_TYPE_DIR;
        }
        return mimeType;
    }

    // Copied from MediaDocumentsProvider with some tweaks
    private Pair<String, String[]> buildSearchSelection(@Nullable Bundle queryArgs,
            @Nullable Set<String> filePaths, @Nullable String parentId) {
        final StringBuilder selection = new StringBuilder();
        final ArrayList<String> selectionArgs = new ArrayList<>();

        if (parentId == null && filePaths != null && filePaths.size() > 0) {
            if (selection.length() > 0) {
                selection.append(" AND ");
            }
            selection.append(DownloadColumns.DATA + " NOT IN (");
            selection.append(TextUtils.join(",", Collections.nCopies(filePaths.size(), "?")));
            selection.append(")");
            selectionArgs.addAll(filePaths);
        }

        if (parentId != null) {
            if (selection.length() > 0) {
            selection.append(" AND ");
            }
            selection.append(DownloadColumns.RELATIVE_PATH + "=?");
            final Pair<String, String> data = getRelativePathAndDisplayNameForDownload(
                    Long.parseLong(parentId));
            selectionArgs.add(data.first + data.second + "/");
        } else {
            if (selection.length() > 0) {
            selection.append(" AND ");
            }
            selection.append(DownloadColumns.RELATIVE_PATH + "=?");
            selectionArgs.add(Environment.DIRECTORY_DOWNLOADS + "/");
        }

        if (queryArgs != null) {
            final boolean shouldExcludeMedia = queryArgs.getBoolean(
                    DocumentsContract.QUERY_ARG_EXCLUDE_MEDIA, false /* defaultValue */);
            if (shouldExcludeMedia) {
                if (selection.length() > 0) {
                    selection.append(" AND ");
                }
                selection.append(DownloadColumns.MIME_TYPE + " NOT LIKE \"image/%\"");
                selection.append(" AND ");
                selection.append(DownloadColumns.MIME_TYPE + " NOT LIKE \"audio/%\"");
                selection.append(" AND ");
                selection.append(DownloadColumns.MIME_TYPE + " NOT LIKE \"video/%\"");
            }

            final String displayName = queryArgs.getString(
                    DocumentsContract.QUERY_ARG_DISPLAY_NAME);
            if (!TextUtils.isEmpty(displayName)) {
                if (selection.length() > 0) {
                selection.append(" AND ");
                }
                selection.append(DownloadColumns.DISPLAY_NAME + " LIKE ?");
                selectionArgs.add("%" + displayName + "%");
            }

            final long lastModifiedAfter = queryArgs.getLong(
                    DocumentsContract.QUERY_ARG_LAST_MODIFIED_AFTER, -1 /* defaultValue */);
            if (lastModifiedAfter != -1) {
                if (selection.length() > 0) {
                selection.append(" AND ");
                }
                selection.append(DownloadColumns.DATE_MODIFIED
                        + " > " + lastModifiedAfter / 1000);
            }

            final long fileSizeOver = queryArgs.getLong(
                    DocumentsContract.QUERY_ARG_FILE_SIZE_OVER, -1 /* defaultValue */);
            if (fileSizeOver != -1) {
                if (selection.length() > 0) {
                selection.append(" AND ");
                }
                selection.append(DownloadColumns.SIZE + " > " + fileSizeOver);
            }

            final String[] mimeTypes = queryArgs.getStringArray(
                    DocumentsContract.QUERY_ARG_MIME_TYPES);
            if (mimeTypes != null && mimeTypes.length > 0) {
                if (selection.length() > 0) {
                selection.append(" AND ");
                }
                selection.append(DownloadColumns.MIME_TYPE + " IN (");
                for (int i = 0; i < mimeTypes.length; ++i) {
                    selection.append("?").append((i == mimeTypes.length - 1) ? ")" : ",");
                    selectionArgs.add(mimeTypes[i]);
                }
            }
        }

        return new Pair<>(selection.toString(), selectionArgs.toArray(new String[0]));
    }

    /**
     * A MatrixCursor that spins up a file observer when the first instance is
     * started ({@link #start()}, and stops the file observer when the last instance
     * closed ({@link #close()}. When file changes are observed, a content change
     * notification is sent on the Downloads content URI.
     *
     * <p>This is necessary as other processes, like ExternalStorageProvider,
     * can access and modify files directly (without sending operations
     * through DownloadStorageProvider).
     *
     * <p>Without this, contents accessible by one a Downloads cursor instance
     * (like the Downloads root in Files app) can become state.
     */
    private static final class DownloadsCursor extends MatrixCursor {

        private static final Object mLock = new Object();
        @GuardedBy("mLock")
        private static int mOpenCursorCount = 0;
        @GuardedBy("mLock")
        private static @Nullable ContentChangedRelay mFileWatcher;
        @GuardedBy("mLock")

        private final ContentResolver mResolver;

        DownloadsCursor(String[] projection, ContentResolver resolver) {
            super(resolveDocumentProjection(projection));
            mResolver = resolver;
        }


        void start() {
            synchronized (mLock) {
                if (mOpenCursorCount++ == 0) {
                    mFileWatcher = new ContentChangedRelay(mResolver,
                            Arrays.asList(getPublicDownloadsDirectory()));
                    mFileWatcher.startWatching();
                }
            }
        }

        @Override
        public void close() {
            super.close();
            synchronized (mLock) {
                if (mOpenCursorCount == 0) {
                    return;
                }
                if (--mOpenCursorCount == 0 && mFileWatcher != null) {
                    mFileWatcher.stopWatching();
                    mFileWatcher = null;
                }
            }
        }
    }

    /**
     * A file observer that notifies on the Downloads content URI(s) when
     * files change on disk.
     */
    private static class ContentChangedRelay extends FileObserver {
        private static final int NOTIFY_EVENTS = ATTRIB | CLOSE_WRITE | MOVED_FROM | MOVED_TO
                | CREATE | DELETE | DELETE_SELF | MOVE_SELF;

        private File[] mDownloadDirs;
        private final ContentResolver mResolver;

        public ContentChangedRelay(ContentResolver resolver, List<File> downloadDirs) {
            super(downloadDirs, NOTIFY_EVENTS);
            mDownloadDirs = downloadDirs.toArray(new File[0]);
            mResolver = resolver;
        }

        @Override
        public void startWatching() {
            super.startWatching();
            if (DEBUG) Log.d(TAG, "Started watching for file changes in: "
                    + Arrays.toString(mDownloadDirs));
        }

        @Override
        public void stopWatching() {
            super.stopWatching();
            if (DEBUG) Log.d(TAG, "Stopped watching for file changes in: "
                    + Arrays.toString(mDownloadDirs));
        }

        @Override
        public void onEvent(int event, String path) {
            if ((event & NOTIFY_EVENTS) != 0) {
                if (DEBUG) Log.v(TAG, "Change detected at path: " + path);
                mResolver.notifyChange(Downloads.Impl.ALL_DOWNLOADS_CONTENT_URI, null, false);
                mResolver.notifyChange(Downloads.Impl.CONTENT_URI, null, false);
            }
        }
    }

    /// M: Add for get DRM method. @{
    //Anuj
    private int getMethod(DrmManagerClient client, String path) {
        int method = 0; // not a drm file
        ContentValues metadata = client.getMetadata(path);
        if (metadata != null && metadata.containsKey(OmaDrmStore.MetadatasColumns.DRM_METHOD)) {
            method = metadata.getAsInteger(OmaDrmStore.MetadatasColumns.DRM_METHOD);
        }
        return method;
    }
    /// @}
}