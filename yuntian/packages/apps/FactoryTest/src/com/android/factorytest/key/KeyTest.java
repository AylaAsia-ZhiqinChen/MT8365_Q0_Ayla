package com.android.factorytest.key;

import android.content.res.Resources;
import android.os.Bundle;
import android.util.TypedValue;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;
import android.widget.GridLayout;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.util.ArrayList;

/**
 * 按键测试
 */
public class KeyTest extends BaseActivity {

    private GridLayout mKeyContainer;
    private ArrayList<Button> mKeyButtons;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_key_test);

        super.onCreate(savedInstanceState);

        mKeyButtons = new ArrayList<Button>();
        mKeyContainer = (GridLayout) findViewById(R.id.key_list);

        initKeyContainer();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        Log.d(this, "onKeyDown=>keyCode: " + keyCode + " event: " + event);
        boolean result = findKeyCodeIndex(keyCode);
        if (!result) {
            return super.onKeyDown(keyCode, event);
        } else {
            return true;
        }
    }

    private void initKeyContainer() {
        Resources res = getResources();
        String[] titles = res.getStringArray(R.array.key_names);
        int[] codes = res.getIntArray(R.array.key_codes);
        int columns = res.getInteger(R.integer.key_test_gridlayout_max_columns);

        // 计算有多少数据
        int maxLength = titles.length;
        if (codes.length < maxLength) {
            maxLength = codes.length;
        }

        // 计算每项尺寸
        int height = res.getDimensionPixelSize(R.dimen.key_test_item_height);
        int maxWidth = res.getDimensionPixelSize(R.dimen.key_test_item_max_width);
        int columnSpace = res.getDimensionPixelSize(R.dimen.key_test_item_column_space);
        int rowSpace = res.getDimensionPixelSize(R.dimen.key_test_item_row_space);
        int screenWidth = res.getDisplayMetrics().widthPixels;
        int width = (screenWidth - (columns * 2 * columnSpace)) / columns;
        if (width > maxWidth) {
            width = maxWidth;
        }

        mKeyContainer.setColumnCount(columns);
        for (int i = 0; i < maxLength; i++) {
            Button button = new Button(this);
            GridLayout.LayoutParams params = new GridLayout.LayoutParams();
            params.width = width;
            params.height = height;
            params.leftMargin = columnSpace;
            params.rightMargin = columnSpace;
            params.topMargin = rowSpace;
            params.bottomMargin = rowSpace;
            button.setLayoutParams(params);
            button.setText(titles[i]);
            TestItem item = new TestItem();
            item.state = false;
            item.code = codes[i];
            button.setTag(item);
            button.setTextSize(TypedValue.COMPLEX_UNIT_SP, res.getInteger(R.integer.key_test_item_text_size));
            button.setAllCaps(false);
            if (titles[i].equals("Trigger")) {
                int key = i;
                button.setOnClickListener(new View.OnClickListener() {
                    public void onClick(View v) {
                        item.state = true;
                        item.code = codes[key];
                        button.setTag(item);
                        button.setBackgroundResource(R.drawable.ic_pass_bg);
                        updateTestState();
                    }
                });
            }
            mKeyContainer.addView(button);
            mKeyButtons.add(button);
        }
    }

    private boolean findKeyCodeIndex(int keyCode) {
        boolean result = false;
        for (int i = 0; i < mKeyButtons.size(); i++) {
            TestItem item = (TestItem) mKeyButtons.get(i).getTag();
            if (!item.state && item.code == keyCode) {
                item.state = true;
                mKeyButtons.get(i).setTag(item);
                mKeyButtons.get(i).setBackgroundResource(R.drawable.ic_pass_bg);
                updateTestState();
                result = true;
                break;
            }
        }
        return result;
    }

    private void updateTestState() {
        boolean success = true;
        for (int i = 0; i < mKeyButtons.size(); i++) {
            TestItem item = (TestItem) mKeyButtons.get(i).getTag();
            if (!item.state) {
                success = false;
                break;
            }
        }
        Log.d(this, "updateTestState=>success: " + success);
        if (success) {
            setPassButtonEnabled(true);
            setTestPass(true);
        }
        if (isAutoTest()) {
            if (isTestPass()) {
                doOnAutoTest();
            }
        }
    }

    class TestItem {
        boolean state;
        int code;
    }
}
