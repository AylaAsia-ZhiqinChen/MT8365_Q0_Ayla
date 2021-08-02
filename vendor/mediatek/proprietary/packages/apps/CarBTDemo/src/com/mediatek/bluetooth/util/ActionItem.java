package com.mediatek.bluetooth.util;

import android.graphics.drawable.Drawable;
import android.view.View.OnClickListener;

/** * Action item, evry item compose by an ImageView and a TextView */
public class ActionItem {
    private Drawable icon;
    private String title;
    private OnClickListener listener;

    /** * constructor*/
    public ActionItem() {
    }

    /** * constructor with Drawable*/
    public ActionItem(Drawable icon) {
        this.icon = icon;
    }

    /** * set title */
    public void setTitle(String title) {
        this.title = title;
    }

    /** * get title* * @return action title */
    public String getTitle() {
        return this.title;
    }

    /** * set icon */
    public void setIcon(Drawable icon) {
        this.icon = icon;
    }

    /** * get icon */
    public Drawable getIcon() {
        return this.icon;
    }

    /** * set listener */
    public void setOnClickListener(OnClickListener listener) {
        this.listener = listener;
    }

    /** * get listener */
    public OnClickListener getListener() {
        return this.listener;
    }
}
