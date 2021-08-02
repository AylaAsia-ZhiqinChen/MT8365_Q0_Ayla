/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright (C) 2008 Esmertec AG.
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

package com.android.mms.transaction;

import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.preference.PreferenceManager;
import android.provider.Telephony.Mms;
import android.provider.Telephony.MmsSms;
import android.provider.Telephony.MmsSms.PendingMessages;
import android.util.Log;

import com.android.mms.LogTag;
import com.android.mms.MmsApp;
import com.android.mms.MmsConfig;
import com.android.mms.util.MmsLog;
import com.android.internal.telephony.PhoneConstants;
import com.google.android.mms.InvalidHeaderValueException;
import com.google.android.mms.MmsException;
import com.google.android.mms.pdu.EncodedStringValue;
import com.google.android.mms.pdu.GenericPdu;
import com.google.android.mms.pdu.ReadRecInd;
import com.google.android.mms.util.SqliteWrapper;
import com.mediatek.android.mms.pdu.MtkPduHeaders;
import com.mediatek.android.mms.pdu.MtkPduPersister;
import com.mediatek.android.mms.pdu.MtkSendReq;
import com.mediatek.mms.appserviceproxy.AppServiceProxy;
import com.mediatek.mms.ext.IOpMmsMessageSenderExt;
import com.mediatek.opmsg.util.OpMessageUtils;
import com.mediatek.setting.MmsPreferenceActivity;
import com.google.android.mms.pdu.SendReq;

public class MmsMessageSender implements MessageSender {
    private static final String TAG = "MmsMessageSender";

    private final Context mContext;
    private final Uri mMessageUri;
    private final long mMessageSize;
    private final int mSubId;
    private IOpMmsMessageSenderExt mOpMmsMessageSenderExt;

    // Default preference values
    private static final boolean DEFAULT_DELIVERY_REPORT_MODE  = false;
    private static final boolean DEFAULT_READ_REPORT_MODE      = false;
    private static final long    DEFAULT_EXPIRY_TIME     = 7 * 24 * 60 * 60;
    private static final int     DEFAULT_PRIORITY        = MtkPduHeaders.PRIORITY_NORMAL;
    private static final String  DEFAULT_MESSAGE_CLASS   = MtkPduHeaders.MESSAGE_CLASS_PERSONAL_STR;

    public MmsMessageSender(Context context, Uri location, long messageSize, int subId) {
        mContext = context;
        mMessageUri = location;
        mMessageSize = messageSize;
        mSubId = subId;
        mOpMmsMessageSenderExt = OpMessageUtils.getOpMessagePlugin()
                       .getOpMmsMessageSenderExt();

        if (mMessageUri == null) {
            throw new IllegalArgumentException("Null message URI.");
        }
    }

    public boolean sendMessage(long token) throws MmsException {
        // Load the MMS from the message uri
        MmsLog.d(MmsApp.TXN_TAG, "MmsMessageSender, sendMessage uri: " + mMessageUri);
        MtkPduPersister p = MtkPduPersister.getPduPersister(mContext);
        GenericPdu pdu = p.load(mMessageUri);

        if (pdu.getMessageType() != MtkPduHeaders.MESSAGE_TYPE_SEND_REQ) {
            throw new MmsException("Invalid message: " + pdu.getMessageType());
        }

        SendReq sendReq = (SendReq) pdu;

        // Update headers.
        updatePreferencesHeaders(sendReq);

        // MessageClass.
        sendReq.setMessageClass(DEFAULT_MESSAGE_CLASS.getBytes());

        // Update the 'date' field of the message before sending it.
        sendReq.setDate(System.currentTimeMillis() / 1000L);

        sendReq.setMessageSize(mMessageSize);

        p.updateHeaders(mMessageUri, sendReq);

        long messageId = ContentUris.parseId(mMessageUri);
        Uri sendUri = null;

        // Move the message into MMS Outbox.
        if (!mMessageUri.toString().startsWith(Mms.Draft.CONTENT_URI.toString())) {
            // If the message is already in the outbox (most likely because we created a "primed"
            // message in the outbox when the user hit send), then we have to manually put an
            // entry in the pending_msgs table which is where TransacationService looks for
            // messages to send. Normally, the entry in pending_msgs is created by the trigger:
            // insert_mms_pending_on_update, when a message is moved from drafts to the outbox.
            ContentValues values = new ContentValues(7);

            values.put(PendingMessages.PROTO_TYPE, MmsSms.MMS_PROTO);
            values.put(PendingMessages.MSG_ID, messageId);
            values.put(PendingMessages.MSG_TYPE, pdu.getMessageType());
            values.put(PendingMessages.ERROR_TYPE, 0);
            values.put(PendingMessages.ERROR_CODE, 0);
            values.put(PendingMessages.RETRY_INDEX, 0);
            values.put(PendingMessages.DUE_TIME, 0);

            Uri pendingUri = SqliteWrapper.insert(mContext, mContext.getContentResolver(),
                    PendingMessages.CONTENT_URI, values);
            MmsLog.d(MmsApp.TXN_TAG, "MmsMessageSender, insert pending messages:" + pendingUri);
            sendUri = mMessageUri;
        } else {
            /// M:Code analyze 002,the uri of pdu moved from draft to outbox @{
            sendUri = p.move(mMessageUri, Mms.Outbox.CONTENT_URI);
            MmsLog.d(MmsApp.TXN_TAG, "MmsMessageSender, move mms from draftBox to outBox");
            /// @}
        }
        updateSubIdForSend(mContext, sendUri, mSubId);
        // Start MMS transaction service
        Intent intent = new Intent();
        intent.setClassName(MmsConfig.MMS_APP_SERVICE_PACKAGE, MmsConfig.TRANSACTION_SERVICE);
        intent.putExtra(TransactionBundle.URI, sendUri.toString());
        intent.putExtra(TransactionBundle.TRANSACTION_TYPE,
                TransactionBundle.SEND_TRANSACTION);
        intent.putExtra(PhoneConstants.SUBSCRIPTION_KEY, mSubId);
        MmsConfig.setMmsConfig(intent, mSubId);
        MmsLog.d(MmsApp.TXN_TAG, "MmsMessageSender, start service here");
        mContext.startService(intent);
        //AppServiceProxy.getInstance(mContext).startServiceByParam(mContext, intent);

        return true;
    }

    // Update the headers which are stored in SharedPreferences.
    private void updatePreferencesHeaders(SendReq sendReq) throws MmsException {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(mContext);

        // Expiry.
        long expiry = prefs.getLong(
                MmsPreferenceActivity.EXPIRY_TIME, DEFAULT_EXPIRY_TIME);
        expiry = mOpMmsMessageSenderExt.updatePreferencesHeaders(expiry, mSubId, prefs);
        sendReq.setExpiry(expiry);

        // Priority.
        /// M:Code analyze 005, modify the logic,set correct priority into header @{
            String priority = prefs.getString(MmsPreferenceActivity.PRIORITY, "Normal");
        if (priority.equals("High")) {
            sendReq.setPriority(MtkPduHeaders.PRIORITY_HIGH);
        } else if (priority.equals("Low")) {
            sendReq.setPriority(MtkPduHeaders.PRIORITY_LOW);
        } else {
            sendReq.setPriority(MtkPduHeaders.PRIORITY_NORMAL);
        }
        /// @}

        // Delivery report.
        boolean dr = prefs.getBoolean(
                mSubId + "_" + MmsPreferenceActivity.MMS_DELIVERY_REPORT_MODE,
                DEFAULT_DELIVERY_REPORT_MODE);

        sendReq.setDeliveryReport(dr ? MtkPduHeaders.VALUE_YES : MtkPduHeaders.VALUE_NO);

        // Read report.
        boolean rr = prefs.getBoolean(mSubId + "_" + MmsPreferenceActivity.READ_REPORT_MODE,
                DEFAULT_READ_REPORT_MODE);

        sendReq.setReadReport(rr ? MtkPduHeaders.VALUE_YES : MtkPduHeaders.VALUE_NO);
        /// M:
        MmsLog.d(MmsApp.TXN_TAG, "MMS DR request=" + dr + "; MMS RR request=" + rr);
    }

    public static void sendReadRec(Context context, String to,
            String messageId, int status, int subId) {
        EncodedStringValue[] sender = new EncodedStringValue[1];
        sender[0] = new EncodedStringValue(to);

        try {
            final ReadRecInd readRec = new ReadRecInd(
                    new EncodedStringValue(MtkPduHeaders.FROM_INSERT_ADDRESS_TOKEN_STR.getBytes()),
                    messageId.getBytes(),
                    MtkPduHeaders.CURRENT_MMS_VERSION,
                    status,
                    sender);

            readRec.setDate(System.currentTimeMillis() / 1000);

            /// M: google jb.mr1 patch, group mms
            Uri uri = MtkPduPersister.getPduPersister(context).persist(readRec,
                    Mms.Outbox.CONTENT_URI, true,
                    MmsPreferenceActivity.getIsGroupMmsEnabled(context), null);
            updateSubIdForSend(context, uri, subId);
            MmsLog.d(MmsApp.TXN_TAG, "sendReadRec uri = " + uri);
            Intent intent = new Intent();
            intent.setClassName(MmsConfig.MMS_APP_SERVICE_PACKAGE, MmsConfig.TRANSACTION_SERVICE);
            intent.putExtra(TransactionBundle.URI, uri.toString());
            intent.putExtra(TransactionBundle.TRANSACTION_TYPE,
                    TransactionBundle.READREC_TRANSACTION);
            intent.putExtra(PhoneConstants.SUBSCRIPTION_KEY, subId);
            context.startService(intent);
            //AppServiceProxy.getInstance(context).startServiceByParam(context, intent);
        } catch (InvalidHeaderValueException e) {
            Log.e(TAG, "Invalide header value", e);
        } catch (MmsException e) {
            Log.e(TAG, "Persist message failed", e);
        }
    }

    private static void updateSubIdForSend(Context context, Uri sendUri, int subId) {
        MmsLog.d(MmsApp.TXN_TAG, "updateSubIdForSend subId = " + subId);
        long msgId = ContentUris.parseId(sendUri);
        //insert sim index in pdu table
        ContentValues values = new ContentValues(1);
        values.put(Mms.SUBSCRIPTION_ID, subId);
        SqliteWrapper.update(context, context.getContentResolver(), sendUri, values, null, null);
        //insert sim index in pending_msgs table
        Uri.Builder uriBuilder = PendingMessages.CONTENT_URI.buildUpon();
        uriBuilder.appendQueryParameter("protocol", "mms");
        uriBuilder.appendQueryParameter("message", String.valueOf(msgId));
        Cursor cursor = SqliteWrapper.query(context, context.getContentResolver(),
                uriBuilder.build(), new String[] { PendingMessages._ID }, null, null, null);
        if (cursor != null) {
            try {
                if (cursor.getCount() == 1 && cursor.moveToFirst()) {
                    ContentValues valuesforPending = new ContentValues();
                    valuesforPending.put(PendingMessages.SUBSCRIPTION_ID, subId);
                    long id = cursor.getLong(0);
                    SqliteWrapper.update(context, context.getContentResolver(),
                            PendingMessages.CONTENT_URI, valuesforPending, PendingMessages._ID
                            + "=" + id, null);
                } else {
                    Log.w(TAG, "get PendingMessage has error");
                }
            } finally {
                cursor.close();
            }
        }
    }
}
