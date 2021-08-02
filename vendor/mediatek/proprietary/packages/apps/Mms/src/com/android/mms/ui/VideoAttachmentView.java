/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright (C) 2008 Esmertec AG.
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.mms.ui;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.MediaMetadataRetriever;
import android.net.Uri;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.ImageView;
import android.widget.LinearLayout;

import com.android.mms.R;
import com.android.mms.util.MmsLog;

import com.mediatek.mms.util.DrmUtilsEx;

import java.util.Map;


/**
 * This class provides an embedded editor/viewer of video attachment.
 */
public class VideoAttachmentView extends LinearLayout implements SlideViewInterface {
    private static final String TAG = "VideoAttachmentView";

    private ImageView mThumbnailView;

    public VideoAttachmentView(Context context) {
        super(context);
    }

    public VideoAttachmentView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onFinishInflate() {
        mThumbnailView = (ImageView) findViewById(R.id.video_thumbnail);
    }

    public void startAudio() {
        // TODO Auto-generated method stub
    }

    public void startVideo() {
        // TODO Auto-generated method stub
    }

    public void setAudio(Uri audio, String name, Map<String, ?> extras) {
        // TODO Auto-generated method stub
    }

    public void setImage(String name, Bitmap bitmap) {
        // TODO Auto-generated method stub
    }

    public void setImageRegionFit(String fit) {
        // TODO Auto-generated method stub
    }

    public void setImageVisibility(boolean visible) {
        // TODO Auto-generated method stub
    }

    public void setText(String name, String text) {
        // TODO Auto-generated method stub
    }

    public void setTextVisibility(boolean visible) {
        // TODO Auto-generated method stub
    }

    public void setVideo(String name, Uri video) {
        try {
            String extName = name.substring(name.lastIndexOf('.') + 1);
            Bitmap bitmap = null;
            if (!extName.equals("dcf")) {
                bitmap = VideoThumbnailCache.getBitmap(name, video.toString());
            }
            if (bitmap == null) {
                bitmap = createVideoThumbnail(mContext, video);
                if (bitmap != null && !extName.equals("dcf")) {
                    VideoThumbnailCache.cacheBitmap(name, video.toString(), bitmap);
                }
            }
            if (null == bitmap) {
                bitmap = BitmapFactory.decodeResource(getResources(),
                        R.drawable.ic_missing_thumbnail_video);
            }
            MmsLog.i(TAG, "Video name is " + name);

            if (extName.equals("dcf")) {
                Bitmap drmBitmap = DrmUtilsEx.getDrmBitmapWithLockIcon(mContext,
                        video, true);
                if (drmBitmap != null) {
                    mThumbnailView.setImageBitmap(drmBitmap);
                } else {
                    mThumbnailView.setImageBitmap(bitmap);
                }
            } else {
                mThumbnailView.setImageBitmap(bitmap);
            }
        } catch (java.lang.OutOfMemoryError e) {
            Log.e(TAG, "setVideo: out of memory: ", e);
        }
    }

    public void setVideoThumbnail(String name, Bitmap thumbnail) {
        mThumbnailView.setImageBitmap(thumbnail);
    }

    public static Bitmap createVideoThumbnail(Context context, Uri uri) {
        Bitmap bitmap = null;
        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
        try {
            retriever.setDataSource(context, uri);
            bitmap = retriever.getFrameAtTime(-1);
        } catch (RuntimeException ex) {
            // Assume this is a corrupt video file.
        } finally {
            try {
                retriever.release();
            } catch (RuntimeException ex) {
                // Ignore failures while cleaning up.
            }
        }
        return bitmap;
    }

    public void setVideoVisibility(boolean visible) {
        // TODO Auto-generated method stub
    }

    public void stopAudio() {
        // TODO Auto-generated method stub
    }

    public void stopVideo() {
        // TODO Auto-generated method stub
    }

    public void reset() {
        // TODO Auto-generated method stub
        mThumbnailView.setImageBitmap(null);
    }

    public void setVisibility(boolean visible) {
        // TODO Auto-generated method stub
    }

    public void pauseAudio() {
        // TODO Auto-generated method stub

    }

    public void pauseVideo() {
        // TODO Auto-generated method stub

    }

    public void seekAudio(int seekTo) {
        // TODO Auto-generated method stub

    }

    public void seekVideo(int seekTo) {
        // TODO Auto-generated method stub

    }

    public void setImage(Uri mUri) {
    }
}
