package com.mediatek.bluetooth.util;

import java.util.ArrayList;

import com.mediatek.bluetooth.R;

import android.content.Context;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.Interpolator;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.TextView;

/**
 * constructor extends PopupWindow
 */
public class QuickActions extends PopupWindow {

    private final View root;
    private final ImageView mArrowUp;
    private final ImageView mArrowDown;
    // private final Animation mTrackAnim;
    private final LayoutInflater inflater;
    private final Context context;

    protected final View anchor;
    protected final PopupWindow window;
    private Drawable background = null;
    protected final WindowManager windowManager;

    public static final int ANIM_GROW_FROM_LEFT = 1;
    public static final int ANIM_GROW_FROM_RIGHT = 2;
    public static final int ANIM_GROW_FROM_CENTER = 3;
    public static final int ANIM_AUTO = 4;

    private int animStyle;
    private boolean animateTrack;
    private ViewGroup mTrack;
    private ArrayList<ActionItem> actionList;

    private RelativeLayout mLayout;

    /**
     * constructor
     * 
     * @param anchor
     *            PopupWindow based on anchor view
     */
    public QuickActions(View anchor) {
        super(anchor);

        this.anchor = anchor;
        this.window = new PopupWindow(anchor.getContext());

        // close window if touch outside popwindow
        window.setTouchInterceptor(new OnTouchListener() {

            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction() == MotionEvent.ACTION_OUTSIDE) {
                    QuickActions.this.window.dismiss();
                    return true;
                }

                return false;
            }
        });

        // get windowManager
        windowManager = (WindowManager) anchor.getContext()
                .getSystemService(Context.WINDOW_SERVICE);

        actionList = new ArrayList<ActionItem>();
        context = anchor.getContext();
        inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        // add layout
        root = (ViewGroup) inflater.inflate(R.layout.quickaction, null);

        // get up and down arrow
        mArrowDown = (ImageView) root.findViewById(R.id.arrow_down);
        mArrowUp = (ImageView) root.findViewById(R.id.arrow_up);
        mLayout = (RelativeLayout) root.findViewById(R.id.total_size);

        setContentView(root);

        /*
         * mTrackAnim = AnimationUtils.loadAnimation(anchor.getContext(),
         * R.anim.rail);
         * 
         * // set anim effort mTrackAnim.setInterpolator(new Interpolator() {
         * 
         * public float getInterpolation(float t) {
         * 
         * final float inner = (t * 1.55f) - 1.1f;
         * 
         * return 1.2f - inner * inner; } });
         */

        mTrack = (ViewGroup) root.findViewById(R.id.tracks);
        animStyle = ANIM_GROW_FROM_CENTER;
        animateTrack = true;
    }

    public PopupWindow getActionsWindow() {
        return window;
    }

    /*
     * 
     */
    public void SetQuickActionsWidth(int width) {
        LayoutParams params = new LayoutParams(width, LayoutParams.WRAP_CONTENT);
        this.mLayout.setLayoutParams(params);
    }

    /**
     * a flag have or not animate
     */
    public void animateTrack(boolean animateTrack) {
        this.animateTrack = animateTrack;
    }

    /**
     * set animate style
     */
    public void setAnimStyle(int animStyle) {
        this.animStyle = animStyle;
    }

    /**
     * add a action
     */
    public void addActionItem(ActionItem action) {
        actionList.add(action);
    }

    /**
     * show pupopwindow
     */
    public void show() {
        preShow();

        int[] location = new int[2];
        // get anchor locatation
        anchor.getLocationOnScreen(location);

        //
        Rect anchorRect = new Rect(location[0], location[1], location[0] + anchor.getWidth(),
                location[1] + anchor.getHeight());

        root.setLayoutParams(new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
        root.measure(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);

        int rootWidth = root.getMeasuredWidth();
        int rootHeight = root.getMeasuredHeight();

        int screenWidth = windowManager.getDefaultDisplay().getWidth();

        // set pupop window location x/y
        int xPos = (screenWidth - rootWidth) / 2;
        int yPos = anchorRect.top - rootHeight;

        boolean onTop = true;

        if (rootHeight > anchorRect.top) {
            yPos = anchorRect.bottom;
            onTop = false;
        }

        showArrow(((onTop) ? R.id.arrow_down : R.id.arrow_up), anchorRect.centerX());

        setAnimationStyle(screenWidth, anchorRect.centerX(), onTop);

        createActionList();

        window.showAtLocation(this.anchor, Gravity.NO_GRAVITY, xPos, yPos);

        // if (animateTrack) mTrack.startAnimation(mTrackAnim);
    }

    /**
     * preshow
     */
    protected void preShow() {
        if (root == null) {
            throw new IllegalStateException("no view");
        }

        if (background == null) {
            window.setBackgroundDrawable(new BitmapDrawable());
        } else {
            window.setBackgroundDrawable(background);
        }

        window.setWidth(WindowManager.LayoutParams.WRAP_CONTENT);
        window.setHeight(WindowManager.LayoutParams.WRAP_CONTENT);
        window.setTouchable(true);
        window.setFocusable(true);
        window.setOutsideTouchable(true);
        window.setContentView(root);
    }

    private void setAnimationStyle(int screenWidth, int requestedX, boolean onTop) {

        int arrowPos = requestedX - mArrowUp.getMeasuredWidth() / 2;

        switch (animStyle) {
        /*
         * case ANIM_GROW_FROM_LEFT: window.setAnimationStyle((onTop) ?
         * R.style.Animations_PopUpMenu_Left :
         * R.style.Animations_PopDownMenu_Left); break;
         * 
         * case ANIM_GROW_FROM_RIGHT: window.setAnimationStyle((onTop) ?
         * R.style.Animations_PopUpMenu_Right :
         * R.style.Animations_PopDownMenu_Right); break;
         */
        case ANIM_GROW_FROM_CENTER:
            window.setAnimationStyle((onTop) ? R.style.Animations_PopUpMenu_Center
                    : R.style.Animations_PopDownMenu_Center);
            break;
        /*
         * case ANIM_AUTO: if (arrowPos <= screenWidth / 4) {
         * window.setAnimationStyle((onTop) ? R.style.Animations_PopUpMenu_Left
         * : R.style.Animations_PopDownMenu_Left); } else if (arrowPos >
         * screenWidth / 4 && arrowPos < 3 * (screenWidth / 4)) {
         * window.setAnimationStyle((onTop) ?
         * R.style.Animations_PopUpMenu_Center :
         * R.style.Animations_PopDownMenu_Center); } else {
         * window.setAnimationStyle((onTop) ?
         * R.style.Animations_PopDownMenu_Right :
         * R.style.Animations_PopDownMenu_Right); }
         * 
         * break;
         */
        }
    }

    /**
     * create action list
     */
    private void createActionList() {
        View view;
        String title;
        Drawable icon;
        OnClickListener listener;
        int index = 1;

        for (int i = 0; i < actionList.size(); i++) {
            title = actionList.get(i).getTitle();
            icon = actionList.get(i).getIcon();
            listener = actionList.get(i).getListener();

            view = getActionItem(title, icon, listener);

            view.setFocusable(true);
            view.setClickable(true);

            mTrack.addView(view, index);

            index++;
        }
    }

    private View getActionItem(String title, Drawable icon, OnClickListener listener) {
        LinearLayout container = (LinearLayout) inflater.inflate(R.layout.action_item, null);
        ImageView img = (ImageView) container.findViewById(R.id.icon);
        TextView text = (TextView) container.findViewById(R.id.title);

        if (icon != null) {
            img.setImageDrawable(icon);
        } else {
            img.setVisibility(View.GONE);
        }

        if (title != null) {
            text.setText(title);
        } else {
            text.setVisibility(View.GONE);
        }

        if (listener != null) {
            container.setOnClickListener(listener);
        }

        return container;
    }

    private void showArrow(int whichArrow, int requestedX) {
        final View showArrow = (whichArrow == R.id.arrow_up) ? mArrowUp : mArrowDown;
        final View hideArrow = (whichArrow == R.id.arrow_up) ? mArrowDown : mArrowUp;

        final int arrowWidth = mArrowUp.getMeasuredWidth();

        showArrow.setVisibility(View.VISIBLE);

        ViewGroup.MarginLayoutParams param = (ViewGroup.MarginLayoutParams) showArrow
                .getLayoutParams();

        param.leftMargin = requestedX - arrowWidth / 2;

        hideArrow.setVisibility(View.INVISIBLE);
    }

}