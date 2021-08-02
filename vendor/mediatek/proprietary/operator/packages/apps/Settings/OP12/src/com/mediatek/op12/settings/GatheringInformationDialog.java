package com.mediatek.op12.settings;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.Gravity;
import android.view.KeyboardShortcutGroup;
import android.view.Menu;
import android.view.Window;
import android.view.WindowManager;

public class GatheringInformationDialog extends Dialog {
    private static final String TAG = "GatheringInformationDialog";
    private Context mContext;
    private ArrayList<DialogConfirm> mDialogConfirmListener;
    public GatheringInformationDialog(Context context) {
        super(context);
        mContext = context;
        mDialogConfirmListener = new ArrayList<DialogConfirm>();
    }

    public void addDialogConfirmListener(DialogConfirm listener){
        mDialogConfirmListener.add(listener);
    }
    public interface DialogConfirm {
        void registerUserConfirm();
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Window dialogWindow = getWindow();
        dialogWindow.setGravity(Gravity.CENTER);
        setContentView(R.layout.gathering_information_dialog);
        WindowManager windowManager = ((Activity)mContext).getWindowManager();
        Display display = windowManager.getDefaultDisplay();
        WindowManager.LayoutParams lp = getWindow().getAttributes();
        lp.width = display.getWidth()*4/5;
        lp.height = display.getHeight()*4/5;
        setCanceledOnTouchOutside(false);
    }

    @Override
    public void onProvideKeyboardShortcuts(List<KeyboardShortcutGroup> data,
        Menu menu, int deviceId) {
    }

    @Override
    public void onPointerCaptureChanged(boolean hasCapture) {
    }

    @Override
    public void dismiss() {
        super.dismiss();
        Log.i(TAG, "dismiss");
        for (DialogConfirm dialogConfirm : mDialogConfirmListener) {
            dialogConfirm.registerUserConfirm();
        }
    }
}