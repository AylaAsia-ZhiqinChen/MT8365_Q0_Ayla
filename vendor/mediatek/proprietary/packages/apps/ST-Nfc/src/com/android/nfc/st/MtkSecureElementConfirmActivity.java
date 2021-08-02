package com.android.nfc.st;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;
import android.widget.TextView;
import com.android.internal.app.AlertActivity;
import com.android.internal.app.AlertController;

public class MtkSecureElementConfirmActivity extends AlertActivity {
    private static final String TAG = "MtkSecureElementConfirmActivity";

    public static final String EXTRA_TITLE = "com.mediatek.nfc.addon.confirm.title";
    public static final String EXTRA_MESSAGE = "com.mediatek.nfc.addon.confirm.message";
    public static final String EXTRA_FIRSTSE = "com.mediatek.nfc.addon.confirm.firstse";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Intent intent = getIntent();
        Log.d(TAG, "onCreate");
        final String title = intent.getStringExtra(EXTRA_TITLE);
        final String message = intent.getStringExtra(EXTRA_MESSAGE);
        final String firstSe = intent.getStringExtra(EXTRA_FIRSTSE);

        /* set to Off first in any case, so if user push "home" this is applied anyway. Then
        if they press OK we will update again. */
        Settings.Global.putString(
                MtkSecureElementConfirmActivity.this.getContentResolver(),
                NfcAddonWrapper.NFC_MULTISE_ACTIVE,
                "Off");

        if (firstSe.equals("")) {

            final AlertController.AlertParams p = mAlertParams;
            p.mIconId = android.R.drawable.ic_dialog_alert;
            p.mTitle = title;
            p.mMessage = message;
            p.mPositiveButtonText = getString(android.R.string.ok);
            setupAlert();

            TextView mMessageTextView = (TextView) getWindow().findViewById(android.R.id.message);
            mMessageTextView.setTextDirection(TextView.TEXT_DIRECTION_LOCALE);

        } else {
            AlertDialog.Builder dialog = new AlertDialog.Builder(this);
            dialog.setCancelable(true);
            dialog.setTitle(title);
            dialog.setMessage(message);
            dialog.setPositiveButton(
                            android.R.string.yes,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id) {
                                    Log.d(TAG, "onClickPostive");
                                    Settings.Global.putString(
                                            MtkSecureElementConfirmActivity.this
                                                    .getContentResolver(),
                                            NfcAddonWrapper.NFC_MULTISE_ACTIVE,
                                            firstSe);
                                    finish();
                                }
                            })
                    .setNegativeButton(
                            android.R.string.no,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    Log.d(TAG, "onClickNegative");
                                    finish();
                                }
                            })
                    .setOnCancelListener(
                            new OnCancelListener() {
                                @Override
                                public void onCancel(DialogInterface dialog) {
                                    Log.d(TAG, "onCancel");
                                    finish();
                                }
                            });

            final AlertDialog alert = dialog.create();

            alert.show();
        }
    }
}
