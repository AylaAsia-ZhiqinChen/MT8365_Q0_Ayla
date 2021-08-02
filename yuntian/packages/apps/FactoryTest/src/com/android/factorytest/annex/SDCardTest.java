package com.android.factorytest.annex;

import android.os.Bundle;
import android.os.Environment;
import android.os.StatFs;
import android.os.storage.StorageManager;
import android.os.storage.StorageVolume;
import android.text.TextUtils;
import android.widget.TextView;
import android.text.format.Formatter;
import java.io.File;
import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;
import android.os.storage.StorageManager;
import android.os.storage.VolumeInfo;
import java.util.Collections;
import android.text.format.Formatter;
import java.util.List;
import android.util.Log;
import java.text.DecimalFormat;
//add by yt_wxc fix SDCard info do not update when sdcard mounted/unmounted in test activity
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
/**
 * ＳDCard测试项
 */
public class SDCardTest extends BaseActivity {

    private TextView mSdcardStateTv;
    private TextView mTotalSizeTv;
    private TextView mFreeSizeTv;
    private StorageManager mStorageManager;
    private double totalBytes,freeBytes,usedBytes;
    private String free;
    private String total;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_sdcard_test);

        super.onCreate(savedInstanceState);

        mStorageManager = (StorageManager) getSystemService(STORAGE_SERVICE);

        mSdcardStateTv = (TextView) findViewById(R.id.sdcard_state);
        mTotalSizeTv = (TextView) findViewById(R.id.sdcard_total_size);
        mFreeSizeTv = (TextView) findViewById(R.id.sdcard_free_size);
        
        //add by yt_wxc fix SDCard info do not update when sdcard mounted/unmounted in test activity begin
        IntentFilter intentfilter = new IntentFilter();
        intentfilter.addAction(Intent.ACTION_MEDIA_MOUNTED);
        intentfilter.addAction(Intent.ACTION_MEDIA_UNMOUNTED);
        intentfilter.addDataScheme("file");
        registerReceiver(mSdCardBroadcastReceiver, intentfilter);
        //add by yt_wxc fix SDCard info do not update when sdcard mounted/unmounted in test activity end
    }

    @Override
    protected void onResume() {
        super.onResume();
        updateSdcardInfo();//add by yt_wxc fix SDCard info do not update when sdcard mounted/unmounted in test activity
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mSdCardBroadcastReceiver);//add by yt_wxc fix SDCard info do not update when sdcard mounted/unmounted in test activity
    }

    private boolean isExSdcardInserted() {
		final List<VolumeInfo> volumes = mStorageManager.getVolumes();
		Collections.sort(volumes, VolumeInfo.getDescriptionComparator());
		for (VolumeInfo vol : volumes) {
		 if(VolumeInfo.TYPE_PUBLIC == vol.getType()){
		   File path = vol.getPath();
		   if(path != null){//add by yt_wxc fixed NullPointerException when the use the bad Sdcard test 
			    totalBytes = path.getTotalSpace();
				freeBytes = path.getFreeSpace();
				usedBytes = totalBytes - freeBytes;
				free = space(freeBytes);
				total = space(totalBytes);
			   }
		   Log.d("SDCardTest-->"," totalBytes = "+totalBytes+" ; freeBytes = "+freeBytes+" ; total = "+total+" ; free = "+free);
		   return true;
		 }
		}
		return false;
    }
    
    private String space(double l){
		double f = l/(1024*1024*1024);
		DecimalFormat fnum = new DecimalFormat("##0.00");
		String volume=fnum.format(f); 
		return volume+"G";
	}
	
	//add by yt_wxc fix SDCard info do not update when sdcard mounted/unmounted in test activity begin
	private void updateSdcardInfo(){
		if (isExSdcardInserted()) {
            mSdcardStateTv.setText(R.string.sdcard_insert);
            mTotalSizeTv.setText(total);
            mFreeSizeTv.setText(free);
            setPassButtonEnabled(true);
        } else {
            mSdcardStateTv.setText(R.string.sdcard_not_insert);
            mTotalSizeTv.setText(R.string.sdcard_unknown_state);
            mFreeSizeTv.setText(R.string.sdcard_unknown_state);
            setPassButtonEnabled(false);
        }
	}
	
	private final BroadcastReceiver mSdCardBroadcastReceiver = new BroadcastReceiver(){
		  @Override
          public void onReceive(Context context, Intent intent) {
                 String action = intent.getAction(); 
                 if(action.equals(Intent.ACTION_MEDIA_MOUNTED) || action.equals(Intent.ACTION_MEDIA_UNMOUNTED)){
					  Log.d("SDCardTest-->","sdcard mounted or unmounted");
					  updateSdcardInfo();
					}
         }
    };
    //add by yt_wxc fix SDCard info do not update when sdcard mounted/unmounted in test activity end
}
