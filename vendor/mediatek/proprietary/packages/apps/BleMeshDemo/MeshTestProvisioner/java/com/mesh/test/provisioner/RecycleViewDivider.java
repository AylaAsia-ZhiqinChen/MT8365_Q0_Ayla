package com.mesh.test.provisioner;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.StaggeredGridLayoutManager;
import android.util.Log;
import android.view.View;

public class RecycleViewDivider extends RecyclerView.ItemDecoration {
    private Paint mPaint;
    private Drawable mDrawable;
    private int mDividerHeight = 2;
    private int mOrientation;
    private static final int[] ATTRS = new int[] { android.R.attr.listDivider };
    public static final int HORIZONTAL_LIST = RecyclerView.HORIZONTAL;
    public static final int VERTICAL_LIST = RecyclerView.VERTICAL;
    public static final int BOTH_SET = 2;

    public RecycleViewDivider(Context context, int orientation) {
        this.setOrientation(orientation);
        final TypedArray a = context.obtainStyledAttributes(ATTRS);
        mDrawable = a.getDrawable(0);
        a.recycle();
    }

    public RecycleViewDivider(Context context, int orientation, int drawableId) {
        this.setOrientation(orientation);
        mDrawable = ContextCompat.getDrawable(context, drawableId);
        mDividerHeight = mDrawable.getIntrinsicHeight();
    }

    public RecycleViewDivider(Context context, int orientation,int dividerHeight, int dividerColor) {
        this.setOrientation(orientation);
        mDividerHeight = dividerHeight;
        mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mPaint.setColor(dividerColor);
        mPaint.setStyle(Paint.Style.FILL);
    }

    public void setOrientation(int orientation) {
        if (orientation < 0 || orientation > 2)
            throw new IllegalArgumentException("invalid orientation");
        mOrientation = orientation;
    }

    @Override
    public void getItemOffsets(Rect outRect, View view, RecyclerView parent,
            RecyclerView.State state) {
        super.getItemOffsets(outRect, view, parent, state);
        RecyclerView.LayoutParams layoutParams = (RecyclerView.LayoutParams) view
                .getLayoutParams();
        int itemPosition = layoutParams.getViewPosition();
        int childCount = parent.getAdapter().getItemCount();
        switch (mOrientation) {
        case BOTH_SET:
            int spanCount = getSpanCount(parent);
            childCount = parent.getAdapter().getItemCount();
            if (isLastRaw(parent, itemPosition, spanCount, childCount))
            {
                outRect.set(0, 0, mDrawable.getIntrinsicWidth(), 0);
            } else if (isLastColum(parent, itemPosition, spanCount, childCount))
            {
                outRect.set(0, 0, 0, mDrawable.getIntrinsicHeight());
            } else
            {
                outRect.set(0, 0, mDrawable.getIntrinsicWidth(),
                        mDrawable.getIntrinsicHeight());
            }
            break;
        case VERTICAL_LIST:
            childCount -= 1;
            outRect.set(0, 0,
                    (itemPosition != childCount) ? mDividerHeight : 0, 0);
            break;
        case HORIZONTAL_LIST:
            childCount -= 1;
            outRect.set(0, 0, 0, (itemPosition != childCount) ? mDividerHeight
                    : 0);
            break;
        }
    }

    @Override
    public void onDraw(Canvas c, RecyclerView parent, RecyclerView.State state) {
        super.onDraw(c, parent, state);
        if (mOrientation == VERTICAL_LIST) {
            drawVertical(c, parent);
        } else if (mOrientation == HORIZONTAL_LIST) {
            drawHorizontal(c, parent);
        } else {
            drawHorizontal(c, parent);
            drawVertical(c, parent);
        }
    }


    private void drawHorizontal(Canvas canvas, RecyclerView parent) {

        final int x = parent.getPaddingLeft();
        final int width = parent.getMeasuredWidth() - parent.getPaddingRight();
        final int childSize = parent.getChildCount();
        for (int i = 0; i < childSize; i++) {
            final View child = parent.getChildAt(i);
            RecyclerView.LayoutParams layoutParams = (RecyclerView.LayoutParams) child
                    .getLayoutParams();
            final int y = child.getBottom() + layoutParams.bottomMargin;
            final int height = y + mDividerHeight;
            if (mDrawable != null) {
                mDrawable.setBounds(x, y, width, height);
                mDrawable.draw(canvas);
            }
            if (mPaint != null) {
                canvas.drawRect(x, y, width, height, mPaint);
            }
        }
    }

    private void drawVertical(Canvas canvas, RecyclerView parent) {
        final int top = parent.getPaddingTop();
        final int bottom = parent.getMeasuredHeight()
                - parent.getPaddingBottom();
        final int childSize = parent.getChildCount();
        for (int i = 0; i < childSize; i++) {
            final View child = parent.getChildAt(i);
            RecyclerView.LayoutParams layoutParams = (RecyclerView.LayoutParams) child
                    .getLayoutParams();
            final int left = child.getRight() + layoutParams.rightMargin;
            final int right = left + mDividerHeight;
            if (mDrawable != null) {
                mDrawable.setBounds(left, top, right, bottom);
                mDrawable.draw(canvas);
            }
            if (mPaint != null) {
                canvas.drawRect(left, top, right, bottom, mPaint);
            }
        }
    }

    private int getSpanCount(RecyclerView parent) {
        int spanCount = -1;
        RecyclerView.LayoutManager layoutManager = parent.getLayoutManager();
        if (layoutManager instanceof GridLayoutManager) {
            spanCount = ((GridLayoutManager) layoutManager).getSpanCount();
        } else if (layoutManager instanceof StaggeredGridLayoutManager) {
            spanCount = ((StaggeredGridLayoutManager) layoutManager)
                    .getSpanCount();
        }
        return spanCount;
    }

    private boolean isLastColum(RecyclerView parent, int pos, int spanCount,
            int childCount) {
        RecyclerView.LayoutManager layoutManager = parent.getLayoutManager();
        if (layoutManager instanceof GridLayoutManager) {
            int orientation = ((GridLayoutManager) layoutManager)
                    .getOrientation();
            if (orientation == StaggeredGridLayoutManager.VERTICAL) {
                if ((pos + 1) % spanCount == 0)
                    return true;
            } else {
                childCount = childCount - childCount % spanCount;
                if (pos >= childCount)
                    return true;
            }
        } else if (layoutManager instanceof StaggeredGridLayoutManager) {
            int orientation = ((StaggeredGridLayoutManager) layoutManager)
                    .getOrientation();
            if (orientation == StaggeredGridLayoutManager.VERTICAL) {
                if ((pos + 1) % spanCount == 0)
                    return true;
            } else {
                childCount = childCount - childCount % spanCount;
                if (pos >= childCount)
                    return true;
            }
        }
        return false;
    }

    private boolean isLastRaw(RecyclerView parent, int pos, int spanCount,
            int childCount) {
        int orientation;
        RecyclerView.LayoutManager layoutManager = parent.getLayoutManager();
        if (layoutManager instanceof GridLayoutManager) {
            childCount = childCount - childCount % spanCount;
            orientation = ((GridLayoutManager) layoutManager).getOrientation();
            if (orientation == StaggeredGridLayoutManager.VERTICAL) {
                childCount = childCount - childCount % spanCount;
                if (pos >= childCount)
                    return true;
            } else {
                if ((pos + 1) % spanCount == 0)
                    return true;
            }
        } else if (layoutManager instanceof StaggeredGridLayoutManager) {
            orientation = ((StaggeredGridLayoutManager) layoutManager)
                    .getOrientation();
            if (orientation == StaggeredGridLayoutManager.VERTICAL) {
                childCount = childCount - childCount % spanCount;
                if (pos >= childCount)
                    return true;
            } else {
                if ((pos + 1) % spanCount == 0)
                    return true;
            }
        }
        return false;
    }
}
