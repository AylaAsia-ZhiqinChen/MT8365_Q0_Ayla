package com.android.factorytest.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Vibrator;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.android.factorytest.R;

/**
 * 马达测试View
 */
public class MotorTestView extends View {

    private Vibrator mVibrator;
    private Paint mPaint;

    private long[] mVibratorTimes;
    private float mTextSize;
    private int mTextColor;

    public MotorTestView(Context context) {
        this(context, null);
    }

    public MotorTestView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MotorTestView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public MotorTestView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);

        mTextSize = getResources().getDimension(R.dimen.test_tip_text_size);
        mTextColor = getResources().getColor(R.color.gray);
        mPaint = new Paint();
        mPaint.setAntiAlias(true);
        mPaint.setDither(true);
        mPaint.setStyle(Paint.Style.FILL);
        mPaint.setStrokeJoin(Paint.Join.ROUND);
        mPaint.setStrokeCap(Paint.Cap.ROUND);
        mVibrator = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
        int[] times = getResources().getIntArray(R.array.motor_test_vibrator_times);
        mVibratorTimes = new long[times.length];
        for (int i = 0; i < times.length; i++) {
            mVibratorTimes[i] = times[i];
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        String tip = getResources().getString(R.string.motor_test_tip);
        mPaint.setTextSize(mTextSize);
        mPaint.setColor(mTextColor);
        float screenWidth = getWidth();
        float screenHeight = getHeight();
        Rect bounds = new Rect();
        mPaint.getTextBounds(tip, 0, tip.length(), bounds);
        canvas.drawText(tip, screenWidth / 2 - bounds.width() / 2, screenHeight / 2 - bounds.height() / 2, mPaint);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                mVibrator.vibrate(mVibratorTimes, 0);
                break;

            case MotionEvent.ACTION_UP:
                mVibrator.cancel();
                break;
        }
        return true;
    }

}
