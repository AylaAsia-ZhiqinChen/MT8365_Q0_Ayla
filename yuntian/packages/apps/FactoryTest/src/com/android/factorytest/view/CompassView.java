package com.android.factorytest.view;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.View;
import android.view.accessibility.AccessibilityEvent;

import java.text.DecimalFormat;

import com.android.factorytest.R;

public class CompassView extends View {

    private Paint markerPaint;
    private Paint textPaint;
    private Paint circlePaint;
    private String northString;
    private String eastString;
    private String southString;
    private String westString;
    private DecimalFormat mDecimalFormat;

    private int mCenterLineWidth;
    private int mCenterLineHeight;
    private int mTriangleWidth;
    private int mTriangleHeight;
    private int mMarkerHeight;
    private int mMainMarkerWidth;
    private int mSubMarkerWidth;
    private int mMarkerAngleTextSize;
    private int mTriangleMargin;
    private int mDirectionMargin;
    private int mAngleMarginTriangle;
    private int mAngleTextSize;
    private float mAngle;


    public CompassView(Context context) {
        super(context);
        initCompassView();
    }

    public CompassView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initCompassView();
    }

    public CompassView(Context context,
                       AttributeSet ats,
                       int defaultStyle) {
        super(context, ats, defaultStyle);
        initCompassView();
    }

    protected void initCompassView() {
        setFocusable(true);

        Resources r = this.getResources();

        mCenterLineWidth = r.getInteger(R.integer.compass_view_center_line_width);
        mCenterLineHeight = r.getInteger(R.integer.compass_view_center_line_height);
        mMarkerHeight = r.getInteger(R.integer.compass_view_marker_height);
        mMainMarkerWidth = r.getInteger(R.integer.compass_view_main_marker_width);
        mSubMarkerWidth = r.getInteger(R.integer.compass_view_sub_marker_width);
        mTriangleWidth = r.getInteger(R.integer.compass_view_triangle_width);
        mTriangleHeight = r.getInteger(R.integer.compass_view_triangle_height);
        mTriangleMargin = r.getInteger(R.integer.compass_view_triangle_margin);
        mDirectionMargin = r.getInteger(R.integer.compass_view_direction_margin);
        mAngleMarginTriangle = r.getInteger(R.integer.compass_view_angle_text_margin_triangle);
        mMarkerAngleTextSize = sp2px(getContext(), r.getInteger(R.integer.compass_view_marker_angle_text_size));
        mAngleTextSize = sp2px(getContext(), r.getInteger(R.integer.compass_view_angle_text_size));
        mAngle = 0.0f;

        mDecimalFormat = new DecimalFormat("#0.0");
        circlePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        circlePaint.setColor(r.getColor(R.color.compass_view_background_color, getContext().getTheme()));
        circlePaint.setStyle(Paint.Style.FILL_AND_STROKE);
        northString = r.getString(R.string.cardinal_north);
        eastString = r.getString(R.string.cardinal_east);
        southString = r.getString(R.string.cardinal_south);
        westString = r.getString(R.string.cardinal_west);
        textPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        textPaint.setColor(r.getColor(R.color.compass_view_marker_text_color, getContext().getTheme()));
        textPaint.setTextSize(mMarkerAngleTextSize);
        markerPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        markerPaint.setColor(r.getColor(R.color.compass_view_marker_color, getContext().getTheme()));
    }

    public void setAngle(float angle) {
        mAngle = angle;
        postInvalidate();
        sendAccessibilityEvent(AccessibilityEvent.TYPE_VIEW_TEXT_CHANGED);
    }

    public float getAngle() {
        return mAngle;
    }

    private int sp2px(Context context, float spValue) {
        final float fontScale = context.getResources().getDisplayMetrics().scaledDensity;
        return (int) (spValue * fontScale + 0.5f);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        // The compass is a circle that fills as much space as possible.
        // Set the measured dimensions by figuring out the shortest boundary,
        // height or width.
        int measuredWidth = measure(widthMeasureSpec);
        int measuredHeight = measure(heightMeasureSpec);
        int d = Math.min(measuredWidth, measuredHeight);
        setMeasuredDimension(d, d);
    }

    private int measure(int measureSpec) {
        int result = 0;
        // Decode the measurement specifications.
        int specMode = MeasureSpec.getMode(measureSpec);
        int specSize = MeasureSpec.getSize(measureSpec);
        if (specMode == MeasureSpec.UNSPECIFIED) {
            // Return a default size of 200 if no bounds are specified.
            result = 200;
        } else {
            // As you want to fill the available space
            // always return the full available bounds.
            result = specSize;
        }
        return result;
    }

    @Override
    protected void onDraw(Canvas canvas) {
		Resources r = getResources();
        int mMeasuredWidth = getMeasuredWidth();
        int mMeasuredHeight = getMeasuredHeight();
        int px = mMeasuredWidth / 2;
        int py = mMeasuredHeight / 2 ;
        int radius = Math.min(px, py) - mCenterLineHeight + mMarkerHeight;

        // Draw the background
        canvas.drawCircle(px, py, radius, circlePaint);

        canvas.save();
        markerPaint.setStrokeWidth(mCenterLineWidth);
        canvas.drawLine(px, py - radius - (mCenterLineHeight - mMarkerHeight), px,
                py - radius + mMarkerHeight, markerPaint);
        canvas.restore();

        // Rotate our perspective so that the ‘top’ is
        // facing the current mAngle.
        canvas.save();
        canvas.rotate(-mAngle, px, py);

        int cardinalX = 0;
        int cardinalY = 0;
        Rect rect = new Rect();
        textPaint.setTextSize(mMarkerAngleTextSize);
        // Draw the marker every 15 degrees and text every 45.
        for (int i = 0; i < 180; i++) {
            canvas.save();
            // Draw the cardinal points
            if (i % 15 == 0) {
                String dirString = "";
                String angle = "";
                switch (i) {
                    case(0) : {
                        dirString = northString;
                        // 画三角形
                        Path trianglePath = new Path();
                        trianglePath.moveTo(px - mTriangleWidth / 2, py - radius - mTriangleMargin);
                        trianglePath.lineTo(px + mTriangleWidth / 2, py - radius - mTriangleMargin);
                        trianglePath.lineTo(px, py - radius - mTriangleHeight);
                        trianglePath.close();
                        markerPaint.setColor(r.getColor(R.color.compass_view_triangle_color, getContext().getTheme()));
                        canvas.drawPath(trianglePath, markerPaint);
                        markerPaint.setColor(r.getColor(R.color.compass_view_marker_color, getContext().getTheme()));

                        textPaint.getTextBounds(dirString, 0, dirString.length(), rect);
                        cardinalX = px - rect.width() / 2;
                        cardinalY = py - radius  + mMarkerHeight + rect.height() + mDirectionMargin;
                        textPaint.setColor(r.getColor(R.color.compass_view_marker_text_color, getContext().getTheme()));
                        canvas.drawText(dirString, cardinalX, cardinalY, textPaint);

                        angle = "0";
                        break;
                    }

                    case(15) :  {
                        angle = "30";
                        break;
                    }

                    case(30) : {
                        angle = "60";
                        break;
                    }

                    case(45) : {
                        dirString = eastString;
                        textPaint.getTextBounds(dirString, 0, dirString.length(), rect);
                        cardinalX = px - rect.width() / 2;
                        cardinalY = py - radius  + mMarkerHeight + rect.height() + mDirectionMargin;
                        textPaint.setColor(r.getColor(R.color.compass_view_marker_text_color, getContext().getTheme()));
                        canvas.drawText(dirString, cardinalX, cardinalY, textPaint);
                        angle = "90";
                        break;
                    }

                    case(60) : {
                        angle = "120";
                        break;
                    }

                    case(75) : {
                        angle = "150";
                        break;
                    }

                    case(90) : {
                        dirString = southString;
                        textPaint.getTextBounds(dirString, 0, dirString.length(), rect);
                        cardinalX = px - rect.width() / 2;
                        cardinalY = py - radius  + mMarkerHeight + rect.height() + mDirectionMargin;
                        textPaint.setColor(r.getColor(R.color.compass_view_marker_text_color, getContext().getTheme()));
                        canvas.drawText(dirString, cardinalX, cardinalY, textPaint);
                        angle = "180";
                        break;
                    }

                    case(105) : {
                        angle = "210";
                        break;
                    }
                    case(120) : {
                        angle = "240";
                        break;
                    }

                    case(135) : {
                        dirString = westString;
                        textPaint.getTextBounds(dirString, 0, dirString.length(), rect);
                        cardinalX = px - rect.width() / 2;
                        cardinalY = py - radius  + mMarkerHeight + rect.height() + mDirectionMargin;
                        textPaint.setColor(r.getColor(R.color.compass_view_marker_text_color, getContext().getTheme()));
                        canvas.drawText(dirString, cardinalX, cardinalY, textPaint);
                        angle = "270";
                        break;
                    }

                    case(150) : {
                        angle = "300";
                        break;
                    }

                    case(165) : {
                        angle = "330";
                        break;
                    }
                }
                textPaint.getTextBounds(angle, 0, angle.length(), rect);
                cardinalX = px - rect.width() / 2;
                cardinalY = py - radius - mAngleMarginTriangle - mTriangleHeight;
                textPaint.setColor(r.getColor(R.color.compass_view_marker_text_color, getContext().getTheme()));
                canvas.drawText(angle, cardinalX, cardinalY, textPaint);
                // Draw a marker.
                markerPaint.setStrokeWidth(mMainMarkerWidth);
                canvas.drawLine(px, py-radius, px, py-radius + mMarkerHeight, markerPaint);
            } else {
                // Draw a marker.
                markerPaint.setStrokeWidth(mSubMarkerWidth);
                canvas.drawLine(px, py-radius, px, py-radius+mMarkerHeight, markerPaint);
            }
            canvas.restore();
            canvas.rotate(2, px, py);
        }
        canvas.restore();

        String angleStr = mDecimalFormat.format(mAngle) + "°";
        textPaint.setTextSize(mAngleTextSize);
        textPaint.getTextBounds(angleStr, 0, angleStr.length(), rect);
        cardinalX = px - rect.width() / 2;
        cardinalY = py + rect.height() / 2;
        textPaint.setColor(r.getColor(R.color.compass_view_angle_text_color, getContext().getTheme()));
        canvas.drawText(angleStr, cardinalX, cardinalY, textPaint);
    }

    @Override
    public boolean dispatchPopulateAccessibilityEvent(final AccessibilityEvent event) {
        super.dispatchPopulateAccessibilityEvent(event);
        if (isShown()) {
            String mAngleStr = String.valueOf(mAngle);
            if (mAngleStr.length() > AccessibilityEvent.MAX_TEXT_LENGTH)
                mAngleStr = mAngleStr.substring(0, AccessibilityEvent.MAX_TEXT_LENGTH);
            event.getText().add(mAngleStr);
            return true;
        }
        return false;
    }
}
