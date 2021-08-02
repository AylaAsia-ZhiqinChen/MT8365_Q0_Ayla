package com.android.factorytest.view;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.android.factorytest.R;

import java.util.ArrayList;

/**
 * 单指触摸测试View
 */
public class SingleFingerTouchView extends View {

    private ArrayList<Rect> mRects;
    private ArrayList<Path> mPaths;
    private Paint mRectPaint;
    private Paint mPathPaint;
    private Path mCurrentPath;
    private OnAllSquarePassListener mListener;

    private int mSquareWidth;
    private int mSquareLineWidth;
    private int mPathLineWidth;
    private float mXPosition;
    private float mYPosition;
    private boolean mIsPass;
    private boolean mEnabledDiagonalTest;

    public SingleFingerTouchView(Context context) {
        this(context, null);
    }

    public SingleFingerTouchView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public SingleFingerTouchView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public SingleFingerTouchView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);

        Resources res = getResources();
        mIsPass = false;
        mRects = new ArrayList<Rect>();
        mPaths = new ArrayList<Path>();
        mSquareWidth = res.getInteger(R.integer.single_finger_touch_test_square_width);
        mSquareLineWidth = res.getInteger(R.integer.single_finger_touch_test_square_line_width);
        mPathLineWidth = res.getInteger(R.integer.single_finger_touch_test_path_width);
        mEnabledDiagonalTest = res.getBoolean(R.bool.enabled_single_finger_touch_test_diagonal);
        mRectPaint = new Paint();
        mRectPaint.setAntiAlias(true);
        mRectPaint.setDither(true);
        mRectPaint.setStrokeJoin(Paint.Join.ROUND);
        mRectPaint.setStrokeCap(Paint.Cap.ROUND);
        mPathPaint = new Paint(mRectPaint);

        mRectPaint.setStyle(Paint.Style.STROKE);
        mRectPaint.setStrokeWidth(mSquareLineWidth);
        mPathPaint.setStyle(Paint.Style.STROKE);
        mPathPaint.setStrokeWidth(mPathLineWidth);
        mPathPaint.setColor(Color.BLACK);
    }


    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        // 在此初始化矩形坐标，避免在View还没有显示时，无法获取View的宽度和高度
        initRects();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        canvas.drawColor(Color.WHITE);

        Rect rect = null;
        // 绘制矩形
        for (int i = 0; i < mRects.size(); i++) {
            rect = mRects.get(i);
            mRectPaint.setColor(Color.RED);
            if (rect.isPass()) {
                mRectPaint.setColor(Color.GREEN);
            }
            canvas.drawRect(rect.getRect(), mRectPaint);
        }

        // 绘制触摸路径
        for (int i = 0; i < mPaths.size(); i++) {
            canvas.drawPath(mPaths.get(i), mPathPaint);
        }
    }

    /**
     * 只处理ACTION_DOWN和ACTION_MOVE事件，以便在手指全部抬起后保留手指触摸位置
     */
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (mIsPass) {
            return super.onTouchEvent(event);
        }
        float x = event.getX();
        float y = event.getY();
        updateRectsStatu(x, y);
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                Path path = new Path();
                path.moveTo(x, y);
                mCurrentPath = path;
                mPaths.add(path);
                break;

            case MotionEvent.ACTION_MOVE:
                mCurrentPath.quadTo(mXPosition, mYPosition, (mXPosition + x) / 2, (mYPosition + y) / 2);
                break;

            case MotionEvent.ACTION_UP:
                mCurrentPath.lineTo(x, y);
                break;
        }
        mXPosition = x;
        mYPosition = y;
        postInvalidate();
        return true;
    }

    /**
     * 更新矩形状态，已经检测是否所有矩形已经通过来判断测试是否通过
     * @param x 手指触摸x坐标
     * @param y 手指触摸y坐标
     */
    private void updateRectsStatu(float x, float y) {
        Rect rect = null;
        for (int i = 0; i < mRects.size(); i++) {
            rect = mRects.get(i);
            if (!rect.isPass()) {
                if (rect.isContainer(x, y)) {
                    rect.setPass(true);
                }
            }
        }

        boolean pass = true;
        for (int i = 0; i < mRects.size(); i++) {
            rect = mRects.get(i);
            if (!rect.isPass()) {
                pass = false;
            }
        }

        if (pass) {
            mIsPass = pass;
            if (mListener != null) {
                mListener.notifyAllSquarePass();
            }
        }
    }

    /**
     * 初始化矩形坐标
     */
    private void initRects() {
        mRects.clear();
        int width = getWidth();
        int height = getHeight();
        int count = width / mSquareWidth;
        // 顶部第一排矩形
        for (int i = 0; i < count; i++) {
            Rect rect = new Rect(1 + i * mSquareWidth, 1, mSquareWidth  - 1 + i * mSquareWidth, mSquareWidth, false);
            mRects.add(rect);
        }

        // 如果矩形无法完全铺满屏幕宽度，则在剩余位置添加一个合适的矩形
        int remainder = width % mSquareWidth;
        if (remainder != 0) {
            Rect rect = new Rect(1 + (count) * mSquareWidth, 1, (count) * mSquareWidth + remainder - 1,
                    mSquareWidth, false);
            mRects.add(rect);
        }

        // 添加屏幕底部矩形
        for (int i = 0; i < count; i++) {
            Rect rect = new Rect(1 + i * mSquareWidth, height - mSquareWidth + 1, mSquareWidth  - 1 + i * mSquareWidth, height - 1, false);
            mRects.add(rect);
        }

        // 如果矩形无法完全铺满屏幕宽度，则在剩余位置添加一个合适的矩形
        remainder = width % mSquareWidth;
        if (remainder != 0) {
            Rect rect = new Rect(1 + (count) * mSquareWidth, height - mSquareWidth + 1, (count) * mSquareWidth + remainder - 1,
                    height - 1, false);
            mRects.add(rect);
        }

        // 添加左边竖排矩形
        count = (height - (mSquareWidth * 2)) / mSquareWidth;
        for (int i = 0; i < count; i++) {
            Rect rect = new Rect(1, mSquareWidth + 1 + i * mSquareWidth, mSquareWidth - 1,
                    mSquareWidth * 2 + i * mSquareWidth - 1, false);
            mRects.add(rect);
        }

        // 如果矩形无法完全铺满屏幕高度，则在剩余位置添加一个合适的矩形
        remainder = (height - (mSquareWidth * 2)) % mSquareWidth;
        if (remainder != 0) {
            Rect rect = new Rect(1, mSquareWidth + count * mSquareWidth + 1, mSquareWidth - 1,
                    mSquareWidth + count * mSquareWidth + remainder - 1, false);
            mRects.add(rect);
        }

        // 添加屏幕右边竖排矩形
        for (int i = 0; i < count; i++) {
            Rect rect = new Rect(width - mSquareWidth + 1, mSquareWidth + 1 + i * mSquareWidth, width - 1,
                    mSquareWidth * 2 + i * mSquareWidth - 1, false);
            mRects.add(rect);
        }

        // 如果矩形无法完全铺满屏幕高度，则在剩余位置添加一个合适的矩形
        remainder = (height - (mSquareWidth * 2)) % mSquareWidth;
        if (remainder != 0) {
            Rect rect = new Rect(width - mSquareWidth + 1, mSquareWidth + count * mSquareWidth + 1, width - 1,
                    mSquareWidth + count * mSquareWidth + remainder - 1, false);
            mRects.add(rect);
        }

        if (mEnabledDiagonalTest) {
            // 屏幕中心点坐标
            int centerX = width / 2;
            int centerY = height / 2;

            // 添加中心矩形
            Rect centerRect = new Rect(centerX - mSquareWidth / 2 + 1, centerY - mSquareWidth / 2 + 1,
                    centerX + mSquareWidth / 2 - 1, centerY + mSquareWidth / 2 - 1, false);
            mRects.add(centerRect);

            // 添加中心矩形左上方的一个矩形
            Rect leftUp = new Rect(centerX - mSquareWidth + 1, centerY - (3 * mSquareWidth) / 2 + 1,
                    centerX - 1, centerY - mSquareWidth / 2 - 1, false);
            mRects.add(leftUp);
            // 添加中心矩形右上方一个矩形
            Rect rightUp = new Rect(centerX + 1, centerY - (3 * mSquareWidth) / 2 + 1,
                    centerX + mSquareWidth - 1, centerY - mSquareWidth / 2 - 1, false);
            mRects.add(rightUp);
            // 添加中心矩形左下方的一个矩形
            Rect leftDown = new Rect(centerX - mSquareWidth + 1, centerY + mSquareWidth / 2 + 1,
                    centerX - 1, centerY + (3 * mSquareWidth) / 2 - 1, false);
            mRects.add(leftDown);
            // 添加中心矩形右下方的一个矩形
            Rect rightDown = new Rect(centerX + 1, centerY + mSquareWidth / 2 + 1,
                    centerX + mSquareWidth - 1, centerY + (3 * mSquareWidth) / 2 - 1, false);
            mRects.add(rightDown);

            // 计算中心矩形上面第一个矩形到屏幕顶面第一排矩形下方可以放置多少个矩形
            count = (centerY - (5 * mSquareWidth) / 2) / mSquareWidth;
            // 根据可以放置的矩形个数计算每个矩形的偏移距离
            int padding = (centerX - 2 * mSquareWidth) / count;

            // 添加中心矩形上方左边第一个矩形到屏幕顶面第一排矩形下方左边的矩形
            for (int i = 0; i <= count; i++) {
                Rect rect = new Rect(centerX - mSquareWidth - padding - i * padding + 1, centerY - (5 * mSquareWidth) / 2 - i * mSquareWidth + 1,
                        centerX - padding - i * padding, centerY - (3 * mSquareWidth) / 2 - i * mSquareWidth - 1 , false);
                mRects.add(rect);
            }

            // 添加中心矩形上方右边第一个矩形到屏幕顶面第一排矩形下方右边的矩形
            for (int i = 0; i <= count; i++) {
                Rect rect = new Rect(centerX + padding + i * padding + 1, centerY - (5 * mSquareWidth) / 2 - i * mSquareWidth + 1,
                        centerX + mSquareWidth + padding + i * padding - 1, centerY - (3 * mSquareWidth) / 2 - i * mSquareWidth - 1 , false);
                mRects.add(rect);
            }

            // 添加中心矩形下方左边第一个矩形到屏幕底部第一排矩形上方左边的矩形
            for (int i = 0; i <= count; i++) {
                Rect rect = new Rect(centerX - mSquareWidth - padding - i * padding + 1, centerY + (3 * mSquareWidth) / 2 + i * mSquareWidth + 1,
                        centerX - padding - i * padding - 1, centerY + (5 * mSquareWidth) / 2 + i * mSquareWidth - 1 , false);
                mRects.add(rect);
            }

            // 添加中心矩形下方右边第一个矩形到屏幕底部第一排矩形上方右边的矩形
            for (int i = 0; i <= count; i++) {
                Rect rect = new Rect(centerX + padding + i * padding + 1, centerY + (3 * mSquareWidth) / 2 + i * mSquareWidth + 1,
                        centerX + mSquareWidth + padding + i * padding - 1, centerY + (5 * mSquareWidth) / 2 + i * mSquareWidth - 1 , false);
                mRects.add(rect);
            }

        }
    }

    public void setOnAllSquarePassListener(OnAllSquarePassListener listener) {
        mListener = listener;
    }

    class Rect {
        private android.graphics.Rect mRect;
        private boolean mIsPass;

        public Rect(android.graphics.Rect rect, boolean isPass) {
            mRect = rect;
            mIsPass = isPass;
        }

        public Rect(int left, int top, int right, int bottom, boolean isPass) {
            mRect = new android.graphics.Rect(left, top, right, bottom);
            mIsPass = isPass;
        }

        public boolean isContainer(float x, float y) {
            if (x >= mRect.left && x <= mRect.right && y >= mRect.top && y <= mRect.bottom) {
                return true;
            }
            return false;
        }

        public android.graphics.Rect getRect() {
            return mRect;
        }

        public void setPass(boolean pass) {
            mIsPass = pass;
        }

        public boolean isPass() {
            return mIsPass;
        }
    }

    public interface OnAllSquarePassListener {
        void notifyAllSquarePass();
    }
}
