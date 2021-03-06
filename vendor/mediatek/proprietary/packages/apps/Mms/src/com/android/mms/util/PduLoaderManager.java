/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.android.mms.util;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.provider.Telephony.Mms;
import android.util.Log;

import com.android.mms.MmsApp;
import com.android.mms.LogTag;
import com.android.mms.model.SlideshowModel;
import com.google.android.mms.MmsException;
import com.google.android.mms.pdu.GenericPdu;
import com.google.android.mms.util.PduCache;
import com.google.android.mms.util.PduCacheEntry;
import com.mediatek.android.mms.pdu.MtkMultimediaMessagePdu;
import com.mediatek.android.mms.pdu.MtkPduPersister;
import com.mediatek.mms.util.PermissionCheckUtil;

import java.util.Set;

/**
 * Primary {@link PduLoaderManager} implementation used by {@link MessagingApplication}.
 * <p>
 * Public methods should only be used from a single thread (typically the UI
 * thread). Callbacks will be invoked on the thread where the PduLoaderManager
 * was instantiated.
 * <p>
 * Uses a thread-pool ExecutorService instead of AsyncTasks since clients may
 * request lots of pdus around the same time, and AsyncTask may reject tasks
 * in that case and has no way of bounding the number of threads used by those
 * tasks.
 * <p>
 * PduLoaderManager is used to asynchronously load mms pdu's and then build a slideshow model
 * from that loaded pdu. Then it will call the passed in callback with the result. This class
 * uses the PduCache built into the mms framework. It also manages a local cache of slideshow
 * models. The slideshow cache uses SoftReferences to hang onto the slideshow.
 *
 * Based on BooksImageManager by Virgil King.
 */
public class PduLoaderManager extends BackgroundLoaderManager {
    private static final String TAG = "Mms:PduLoaderManager";

    private static final boolean DEBUG_DISABLE_CACHE = false;
    private static final boolean DEBUG_DISABLE_PDUS = false;
    private static final boolean DEBUG_LONG_WAIT = false;

    private static PduCache mPduCache;
    private final MtkPduPersister mMtkPduPersister;
    private final SimpleCache<Uri, SlideshowModel> mSlideshowCache;
    private final Context mContext;

    private BroadcastReceiver mMmsContentChangedReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            MmsLog.d(TAG, "onReceive action = " + action);
            if (!PermissionCheckUtil.checkRequiredPermissions(context)) {
                MmsLog.d(MmsApp.TXN_TAG, "SmsRejectedReceiver: onReceive()"
                        + " no runtime permissions intent=" + intent);
                return;
            }
            
            if (action.equals(Mms.Intents.CONTENT_CHANGED_ACTION)) {
                final Intent mIntent = intent;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        Uri changed = (Uri) mIntent.getParcelableExtra(Mms.Intents.DELETED_CONTENTS);
                        if (changed != null) {
                            removePdu(changed);
                            MmsLog.d(MmsApp.TXN_TAG, "Mms.Intents.CONTENT_CHANGED_ACTION: " + changed);
                        }
                    }
                }).start();
            }
        }
    };

    public PduLoaderManager(final Context context) {
        super(context);

        mSlideshowCache = new SimpleCache<Uri, SlideshowModel>(8, 16, 0.75f, true);
        mPduCache = PduCache.getInstance();
        mMtkPduPersister = MtkPduPersister.getPduPersister(context);
        mContext = context;
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(Mms.Intents.CONTENT_CHANGED_ACTION);
        context.registerReceiver(mMmsContentChangedReceiver, intentFilter);
    }

    public ItemLoadedFuture getPdu(Uri uri, boolean requestSlideshow,
            final ItemLoadedCallback<PduLoaded> callback) {
        if (uri == null) {
            throw new NullPointerException();
        }

        PduCacheEntry cacheEntry = null;
        synchronized (mPduCache) {
            if (!mPduCache.isUpdating(uri)) {
                cacheEntry = mPduCache.get(uri);
            }
        }
        final SlideshowModel slideshow = (requestSlideshow && !DEBUG_DISABLE_CACHE) ?
                mSlideshowCache.get(uri) : null;

        final boolean slideshowExists = (!requestSlideshow || slideshow != null);
        final boolean pduExists = (cacheEntry != null && cacheEntry.getPdu() != null);
        final boolean taskExists = mPendingTaskUris.contains(uri);
        final boolean newTaskRequired = (!pduExists || !slideshowExists) && !taskExists;
        final boolean callbackRequired = (callback != null);

        if (pduExists && slideshowExists) {
            if (callbackRequired) {
                PduLoaded pduLoaded = new PduLoaded(cacheEntry.getPdu(), slideshow);
                callback.onItemLoaded(pduLoaded, null);
            }
            return new NullItemLoadedFuture();
        }

        if (callbackRequired) {
            addCallback(uri, callback);
        }

        if (newTaskRequired) {
            mPendingTaskUris.add(uri);
            Runnable task = new PduTask(uri, requestSlideshow);
            mExecutor.execute(task);
        }
        return new ItemLoadedFuture() {
            private boolean mIsDone;

            public void cancel(Uri uri) {
                cancelCallback(callback);
                removePdu(uri);     // the pdu and/or slideshow might be half loaded. Make sure
                                    // we load fresh the next time this uri is requested.
            }

            public void setIsDone(boolean done) {
                mIsDone = done;
            }

            public boolean isDone() {
                return mIsDone;
            }
        };
    }

    @Override
    public void clear() {
        super.clear();

        synchronized (mPduCache) {
            mPduCache.purgeAll();
        }
        mSlideshowCache.clear();
    }

    public void removePdu(Uri uri) {
        if (Log.isLoggable(TAG, Log.DEBUG)) {
            MmsLog.dpi(TAG, "removePdu: " + uri);
        }
        if (uri != null) {
            synchronized (mPduCache) {
                mPduCache.purge(uri);
            }
            mSlideshowCache.remove(uri);
        }
    }

    public String getTag() {
        return TAG;
    }

    public class PduTask implements Runnable {
        private final Uri mUri;
        private final boolean mRequestSlideshow;

        public PduTask(Uri uri, boolean requestSlideshow) {
            if (uri == null) {
                throw new NullPointerException();
            }
            mUri = uri;
            mRequestSlideshow = requestSlideshow;
        }

        /** {@inheritDoc} */
        public void run() {
            if (DEBUG_DISABLE_PDUS) {
                return;
            }
            if (DEBUG_LONG_WAIT) {
                try {
                    Thread.sleep(10000);
                } catch (InterruptedException e) {
                }
            }
            GenericPdu pdu = null;
            SlideshowModel slideshow = null;
            Throwable exception = null;
            try {
                pdu = mMtkPduPersister.load(mUri);
                if (pdu != null && mRequestSlideshow) {
                    slideshow = SlideshowModel.createFromPduBody(mContext,
                            ((MtkMultimediaMessagePdu) pdu).getBody());
                }
            } catch (final MmsException e) {
                Log.e(TAG, "MmsException loading uri: " + mUri, e);
                exception = e;
            }
            final GenericPdu resultPdu = pdu;
            final SlideshowModel resultSlideshow = slideshow;
            final Throwable resultException = exception;
            mCallbackHandler.post(new Runnable() {
                public void run() {
                    final Set<ItemLoadedCallback> callbacks = mCallbacks.get(mUri);
                    if (callbacks != null) {
                        // Make a copy so that the callback can unregister itself
                        for (final ItemLoadedCallback<PduLoaded> callback : asList(callbacks)) {
                            if (Log.isLoggable(TAG, Log.DEBUG)) {
                                Log.d(TAG, "Invoking pdu callback " + callback);
                            }
                            PduLoaded pduLoaded = new PduLoaded(resultPdu, resultSlideshow);
                            callback.onItemLoaded(pduLoaded, resultException);
                        }
                    }
                    // Add the slideshow to the soft cache if the load succeeded
                    if (resultSlideshow != null) {
                        mSlideshowCache.put(mUri, resultSlideshow);
                    }

                    mCallbacks.remove(mUri);
                    mPendingTaskUris.remove(mUri);

                    if (Log.isLoggable(LogTag.PDU_CACHE, Log.DEBUG)) {
                        MmsLog.dpi(TAG, "Pdu task for " + mUri + "exiting; "
                                + mPendingTaskUris.size() + " remain");
                    }
                }
            });
        }
    }

    public static class PduLoaded {
        public final GenericPdu mPdu;
        public final SlideshowModel mSlideshow;

        public PduLoaded(GenericPdu pdu, SlideshowModel slideshow) {
            mPdu = pdu;
            mSlideshow = slideshow;
        }
    }
}
