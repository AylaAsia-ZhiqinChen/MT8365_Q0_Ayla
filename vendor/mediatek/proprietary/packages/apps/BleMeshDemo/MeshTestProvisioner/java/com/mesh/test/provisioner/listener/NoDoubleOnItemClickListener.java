package com.mesh.test.provisioner.listener;

import android.view.View;
import android.widget.AdapterView;

import java.util.Calendar;

public abstract class NoDoubleOnItemClickListener implements AdapterView.OnItemClickListener {
    public static final int MIN_CLICK_DELAY_TIME = 1000;
    private long lastClickTime = 0;

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        long currentTime = Calendar.getInstance().getTimeInMillis();
        if (currentTime - lastClickTime > MIN_CLICK_DELAY_TIME) {
            lastClickTime = currentTime;
            onNoDoubleClick(parent, view, position, id);
        }

    }

    public abstract void onNoDoubleClick(AdapterView<?> parent, View view, int position, long id);
}
