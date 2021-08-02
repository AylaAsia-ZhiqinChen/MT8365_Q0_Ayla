package com.android.factorytest.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.android.factorytest.R;

/**
 * 屏幕颜色测试View
 */
public class ScreenColorView extends View {

    private Paint mPaint;
    private OnScreenColorTestCompletedListener mListener;

    private int[] mTestColors;
    private int mTestColorIndex;
    private int mTextColor;
    private float mTextSize;

    public ScreenColorView(Context context) {
        this(context, null);
    }

    public ScreenColorView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public ScreenColorView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public ScreenColorView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);

        mTestColorIndex = 0;
        mTestColors = getResources().getIntArray(R.array.screen_color_test_colors);
        mTextSize = getResources().getDimension(R.dimen.test_tip_text_size);
        mTextColor = getResources().getColor(R.color.test_tip_text_color);
        mPaint = new Paint();
        mPaint.setAntiAlias(true);
        mPaint.setDither(true);
        mPaint.setStyle(Paint.Style.FILL);
        mPaint.setStrokeJoin(Paint.Join.ROUND);
        mPaint.setStrokeCap(Paint.Cap.ROUND);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (mTestColorIndex < mTestColors.length) {
            int color = mTestColors[mTestColorIndex];
            canvas.drawColor(color);
        }

        mPaint.setTextSize(mTextSize);
        mPaint.setColor(mTextColor);
        float screenWidth = getWidth();
        float screenHeight = getHeight();
        Rect bounds = new Rect();
        String tip = getResources().getString(R.string.screen_color_test_continue_tip);
        if (mTestColorIndex >= mTestColors.length) {
            tip = getResources().getString(R.string.screen_color_test_finish_tip);
        }
        mPaint.getTextBounds(tip, 0, tip.length(), bounds);
        canvas.drawText(tip, screenWidth / 2 - bounds.width() / 2, screenHeight / 2 - bounds.height() / 2, mPaint);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
                mTestColorIndex++;
                if (mTestColorIndex >= mTestColors.length) {
                    if (mListener != null) {
                        mListener.notifyScreenColorTestCompleted();
                    }
                }
                postInvalidate();
                break;
        }
        return true;
    }

    public void setOnScreenColorTestCompletedListener(OnScreenColorTestCompletedListener listener) {
        mListener = listener;
    }

    public interface OnScreenColorTestCompletedListener {
        void notifyScreenColorTestCompleted();
    }
}
