package com.android.factorytest.camera;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.ScaleAnimation;
import android.view.animation.AnimationSet;

import com.android.factorytest.R;

public class FocusCirceView extends View {

    private Paint paint;
    private static final String TAG = "FocusCirceView";
    private float mX = getWidth() / 2; //默认
    private float mY = getHeight() / 2;

    public FocusCirceView(Context context) {
        super(context);
    }

    public FocusCirceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void setPoint(float x, float y) {
        this.mX = x;
        this.mY = y;
    }

    //重写draw方法
    @Override
    public void draw(Canvas canvas) {
        super.draw(canvas);
        //实例化画笔
        paint = new Paint();
        //给画笔设置颜色 #f4f4f4
        paint.setColor(Color.parseColor(getContext().getString(R.color.green)));
        //设置画笔属性
        paint.setStyle(Paint.Style.STROKE);//空心圆
        paint.setStrokeWidth(1);
        Log.d(TAG, "draw: " + "width:" + getWidth() + "___height:" + getHeight());
        canvas.drawCircle(mX, mY, 20, paint);
        canvas.drawCircle(mX, mY, 60, paint);
    }

    public void deleteCanvas() {
        if (paint != null) {
            paint.reset();
        }
        invalidate();
        setVisibility(View.GONE);
    }

    /***
     * 缩放动画
     */
    public void myViewScaleAnimation(View myView, float mX, float mY) {
        AnimationSet animationSet = new AnimationSet(true);
        ScaleAnimation scaleAnimation = new ScaleAnimation(1.2f, 1f, 1.2f, 1f, mX, mY);
        scaleAnimation.setDuration(1000);
        animationSet.addAnimation(scaleAnimation);
        animationSet.setFillAfter(false); //让其保持动画结束时的状态。
        myView.startAnimation(animationSet);
        myView.setVisibility(View.GONE);
    }
}