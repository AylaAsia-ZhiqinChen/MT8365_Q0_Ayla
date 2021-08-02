package com.android.factorytest.annex;

import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.os.StatFs;
import android.os.storage.DiskInfo;
import android.os.storage.StorageEventListener;
import android.os.storage.StorageManager;
import android.os.storage.VolumeInfo;
import android.preference.Preference;
import android.text.TextUtils;
import android.text.format.Formatter;
import android.widget.TextView;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.io.File;
import java.io.FileInputStream;
import java.util.Collections;
import java.util.List;

public class OTGTest extends BaseActivity {

    private TextView mOtgStateTv;
    private TextView mOtgNameTv;
    private TextView mTotalSizeTv;
    private TextView mFreeSizeTv;
    private StorageManager mStorageManager;

    private String mOtgDiskPath;
    private String mOtgDiskName;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_otg_test);

        super.onCreate(savedInstanceState);

        mStorageManager = (StorageManager) getSystemService(STORAGE_SERVICE);

        mOtgStateTv = (TextView) findViewById(R.id.otg_disk_state);
        mOtgNameTv = (TextView) findViewById(R.id.otg_disk_name);
        mTotalSizeTv = (TextView) findViewById(R.id.otg_disk_total_size);
        mFreeSizeTv = (TextView) findViewById(R.id.otg_disk_free_size);
    }

    @Override
    protected void onResume() {
        super.onResume();
        mStorageManager.registerListener(mStorageListener);
        refresh();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mStorageManager.unregisterListener(mStorageListener);
    }

    private void refresh() {
        final List<VolumeInfo> volumes = mStorageManager.getVolumes();
        Collections.sort(volumes, VolumeInfo.getDescriptionComparator());

		mOtgDiskPath = "";
		mOtgDiskName = "";
        for (VolumeInfo vol : volumes) {
            if (vol.getType() == VolumeInfo.TYPE_PUBLIC) {
                if (vol.isUSBOTG() && vol.isMountedReadable()) {
                    mOtgDiskPath = vol.path;
                    mOtgDiskName = mStorageManager.getBestVolumeDescription(vol);
                }
            }
        }
        if (!TextUtils.isEmpty(mOtgDiskPath)) {
            mOtgStateTv.setText(R.string.otg_disk_insert);
            mOtgNameTv.setText((mOtgDiskName != null) ? mOtgDiskName : getString(R.string.otg_disk_unknown_state));
            String totalSizeStr = getOtgDiskTotalSize();
            mTotalSizeTv.setText(totalSizeStr);
            String freeSizeStr = getOtgDiskFreeSize();
            mFreeSizeTv.setText(freeSizeStr);
            setPassButtonEnabled(true);
            setTestPass(true);
        } else {
            mOtgStateTv.setText(R.string.otg_disk_not_insert);
            mOtgNameTv.setText(R.string.otg_disk_unknown_state);
            mTotalSizeTv.setText(R.string.otg_disk_unknown_state);
            mFreeSizeTv.setText(R.string.otg_disk_unknown_state);
        }
        if (isAutoTest()) {
            if (isTestPass()) {
                doOnAutoTest();
            }
        }
    }

    private String getOtgDiskTotalSize() {
		try {
			StatFs statfs = new StatFs(mOtgDiskPath);
			long blockCount = statfs.getBlockCount();
			long blockSize = statfs.getBlockSize();
			long totalSize = blockCount * blockSize;
			return Formatter.formatFileSize(this, totalSize);
		} catch (Exception e) {
			Log.e(this, "getOtgDiskTotalSize=>error: ", e);
		}
		return "--";
    }

    private String getOtgDiskFreeSize() {
		try {
			StatFs statfs = new StatFs(mOtgDiskPath);
			long blockSize = statfs.getBlockSize();
			long availableBlocks = statfs.getAvailableBlocks();
			long freeSize = availableBlocks * blockSize;
			return Formatter.formatFileSize(this, freeSize);
        } catch (Exception e) {
			Log.e(this, "getOtgDiskFreeSize=>error: ", e);
		}
		return "--";
    }

    private static boolean isInteresting(VolumeInfo vol) {
        switch(vol.getType()) {
            case VolumeInfo.TYPE_PRIVATE:
            case VolumeInfo.TYPE_PUBLIC:
                return true;
            default:
                return false;
        }
    }

    private final StorageEventListener mStorageListener = new StorageEventListener() {
        @Override
        public void onVolumeStateChanged(VolumeInfo vol, int oldState, int newState) {
            if (isInteresting(vol)) {
                refresh();
            }
        }
        /// M: ALPS02316229 refresh UI when plug in or out SD card. @{
        @Override
        public void onDiskScanned(DiskInfo disk, int volumeCount) {
            refresh();
        }

        @Override
        public void onDiskDestroyed(DiskInfo disk) {
            refresh();
        }
        /// @}
    };
}
