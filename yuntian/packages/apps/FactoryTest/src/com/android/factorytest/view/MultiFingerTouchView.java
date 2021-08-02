package com.android.factorytest.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.util.ArrayList;

/**
 * 多指触摸测试view
 */
public class MultiFingerTouchView extends View {

    private static final int MAX_FINGERS = 10;//mod by yt_wxc

    private ArrayList<PointF> mPoints;
    private Paint mPaint;
    private Bitmap mBackgroupBitmap;
    private OnFingerCountChangedListener mListener;

    private int[] mFingerColors;
    private int mBackgroundColor;
    private int mFingerRadiu;
    private float mTextSize;
    private float mTextPadding;

    public MultiFingerTouchView(Context context) {
        this(context, null);
    }

    public MultiFingerTouchView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MultiFingerTouchView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public MultiFingerTouchView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        mPoints = new ArrayList<PointF>();
        mPaint = new Paint();
        mPaint.setAntiAlias(true);
        mPaint.setDither(true);
        mPaint.setStyle(Paint.Style.FILL);
        mPaint.setStrokeJoin(Paint.Join.ROUND);
        mPaint.setStrokeCap(Paint.Cap.ROUND);
        mFingerColors = getResources().getIntArray(R.array.multi_finger_touch_finger_colors);
        mFingerRadiu = getResources().getDimensionPixelSize(R.dimen.multi_finger_touch_finger_radius);
        mTextSize = getResources().getDimension(R.dimen.multi_finger_touch_position_text_size);
        mTextPadding = getResources().getDimensionPixelSize(R.dimen.multi_finger_touch_text_padding);
        mBackgroundColor = getResources().getColor(R.color.black_gray);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        mBackgroupBitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
        mBackgroupBitmap.eraseColor(mBackgroundColor);
        mPaint.setColor(mBackgroundColor);
        canvas.drawBitmap(mBackgroupBitmap, getWidth(), getHeight(), mPaint);

        for (int i = 0; i < mPoints.size(); i++) {
            mPaint.setColor(mFingerColors[i]);
            canvas.drawCircle(mPoints.get(i).x, mPoints.get(i).y, mFingerRadiu, mPaint);
        }

        String text = null;
        float textHeight = 0.0f;
        float textWidth = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        float xPosition = 0.0f;
        float yPosition = 0.0f;
        float screenWidth = getResources().getDisplayMetrics().widthPixels;
        float screenHeight = getResources().getDisplayMetrics().heightPixels;
        Rect bounds = new Rect();
        for (int i = 0; i < mPoints.size(); i++) {
            x = mPoints.get(i).x;
            y = mPoints.get(i).y;
            text = "X: " + x + ", Y: " + y;
            mPaint.setTextSize(mTextSize);
            mPaint.setColor(mFingerColors[mFingerColors.length - i - 1]);
            Paint.FontMetrics fm = mPaint.getFontMetrics();
            mPaint.getTextBounds(text, 0, text.length(), bounds);
            textHeight = bounds.height() + mTextPadding;
            textWidth = bounds.width();
            if ((x - textWidth / 2) < 0.1f) {//X坐标判断,移动到边沿另作处理
                xPosition = 1;
            } else if ((x + textWidth / 2) > screenWidth) {
                xPosition = screenWidth - textWidth;
            } else {
                xPosition = x - textWidth / 2;
            }

            if ((y - (mFingerRadiu + bounds.height() * 2 + mTextPadding)) < 0.1f) {//Y坐标判断,移动到边沿另作处理
                yPosition = y + mFingerRadiu + textHeight;
            } else {
                yPosition = y - (mFingerRadiu + textHeight);
            }
            canvas.drawText(text, xPosition, yPosition, mPaint);
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int pointerCount = event.getPointerCount();
        int cappedPointerCount = pointerCount > MAX_FINGERS ? MAX_FINGERS : pointerCount;
        Log.d(this, "onTouchEvent=>count: " + cappedPointerCount + " action: " + event.getAction() + " maked: " + event.getActionMasked());

        if (mListener != null) {
            mListener.notifyFingerCountChanged(cappedPointerCount);
        }

        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                mPoints.clear();
                for (int i = 0; i < cappedPointerCount; i++) {
                    int id = event.getPointerId(i);
                    int index = event.findPointerIndex(id);
                    try {
                        mPoints.add(new PointF(event.getX(index), event.getY(index)));
                    } catch (IllegalArgumentException e) {
                        e.printStackTrace();
                    }
                }
                break;

            case MotionEvent.ACTION_MOVE:
                mPoints.clear();
                for (int i = 0; i < cappedPointerCount; i++) {
                    int id = event.getPointerId(i);
                    int index = event.findPointerIndex(id);
                    Log.d(this, "onTouchEvent(MOVE)=>i: " + i + " id: " + id + " index: " + index);
                    try {
                        mPoints.add(new PointF(event.getX(index), event.getY(index)));
                    } catch (IllegalArgumentException e) {
                        e.printStackTrace();
                    }
                }
                break;

            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                mPoints.clear();
                break;
        }
        invalidate();
        return true;
    }

    public void setOnFingerCountChangedListener(OnFingerCountChangedListener listener) {
        mListener = listener;
    }

    public interface OnFingerCountChangedListener {
        void notifyFingerCountChanged(int count);
    }
}
