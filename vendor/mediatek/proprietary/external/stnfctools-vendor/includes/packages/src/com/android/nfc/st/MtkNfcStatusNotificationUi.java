package com.android.nfc.st;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import com.android.nfc.R;

public class MtkNfcStatusNotificationUi {
    private static final String TAG = "MtkNfcStatusNotificationUi";

    private static final int UNIQUE_NOTIF_ID = 50000;
    private static String channel_id = "NFC_channel";
    private Context mContext;
    private NotificationChannel mChannel;
    private NotificationManager mNotificationManager;

    private MtkNfcStatusNotificationUi(Context context) {
        mContext = context;
        mNotificationManager =
                (NotificationManager) mContext.getSystemService(Context.NOTIFICATION_SERVICE);

        CharSequence chan_name = mContext.getString(R.string.nfcUserLabel);
        String description = mContext.getString(R.string.app_name);
        int importance = NotificationManager.IMPORTANCE_LOW;
        mChannel = new NotificationChannel(channel_id, chan_name, importance);
        mChannel.setDescription(description);
        mNotificationManager.createNotificationChannel(mChannel);
    }

    private static MtkNfcStatusNotificationUi mSingleton;

    public static void createSingleton(Context context) {
        if (mSingleton == null) {
            mSingleton = new MtkNfcStatusNotificationUi(context);
        }
    }

    public static MtkNfcStatusNotificationUi getInstance() {
        return mSingleton;
    }

    public void showNotification() {
        String msg = "";
        String title = "NFC";
        // title = mContext.getString(R.string.nfcUserLabel);
        msg = mContext.getString(R.string.mtk_notification_string);
        Intent intent = new Intent("android.settings.NFC_SETTINGS");
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
        PendingIntent pendingIntent = PendingIntent.getActivity(mContext, 0, intent, 0);
        Notification n = createNotification(title, msg, pendingIntent);
        n.flags |= Notification.FLAG_NO_CLEAR; // NFC status bar is persistent
        mNotificationManager.notify(UNIQUE_NOTIF_ID, n);
    }

    public void hideNotification() {
        mNotificationManager.cancel(UNIQUE_NOTIF_ID);
    }

    private Notification createNotification(
            CharSequence title, CharSequence infoWord, PendingIntent intent) {

        return new Notification.Builder(mContext, channel_id)
                .setContentTitle(title)
                .setContentText(infoWord)
                .setSmallIcon(R.drawable.mtk_nfc)
                // .addAction(android.R.drawable.ic_delete, cancelWord, intent)
                .setContentIntent(intent)
                // .setProgress(0, 0, true)
                // .setPriority(100)
                .build();
    }
}
