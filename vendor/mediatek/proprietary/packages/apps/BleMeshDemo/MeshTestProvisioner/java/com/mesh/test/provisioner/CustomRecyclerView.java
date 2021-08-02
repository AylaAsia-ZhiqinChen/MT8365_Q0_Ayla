package com.mesh.test.provisioner;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.res.TypedArray;
import android.os.Build;
import android.support.annotation.Nullable;
import android.support.v4.view.ViewCompat;
import android.support.v7.widget.RecyclerView;
import android.util.AttributeSet;
import android.view.InputDevice;
import android.view.MotionEvent;
import android.view.View.MeasureSpec;

public class CustomRecyclerView extends RecyclerView {
    private float mVerticalScrollFactor = 20.f;

    public CustomRecyclerView(Context context) {
        this(context,null);
    }

    public CustomRecyclerView(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public CustomRecyclerView(Context context, @Nullable AttributeSet attrs,
            int defStyle) {
        super(context, attrs, defStyle);
    }


    @TargetApi(Build.VERSION_CODES.HONEYCOMB_MR1)
    @Override
    public boolean onGenericMotionEvent(MotionEvent event) {
        if ((event.getSource() & InputDevice.SOURCE_CLASS_POINTER) != 0) {
            if (event.getAction() == MotionEvent.ACTION_SCROLL
                    && getScrollState() == SCROLL_STATE_IDLE) {
                final float vscroll = event
                        .getAxisValue(MotionEvent.AXIS_VSCROLL);
                if (vscroll != 0) {
                    final int delta = -1
                            * (int) (vscroll * mVerticalScrollFactor);
                    if (ViewCompat
                            .canScrollVertically(this, delta > 0 ? 1 : -1)) {
                        scrollBy(0, delta);
                        return true;
                    }
                }
            }
        }
        return super.onGenericMotionEvent(event);
    }
}
