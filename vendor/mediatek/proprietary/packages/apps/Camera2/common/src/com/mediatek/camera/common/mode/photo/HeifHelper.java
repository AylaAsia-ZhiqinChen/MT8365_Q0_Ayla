package com.mediatek.camera.common.mode.photo;

import android.content.ContentValues;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.media.Image;
import android.os.Environment;
import android.provider.MediaStore;
import com.mediatek.camera.common.ICameraContext;
import com.mediatek.camera.common.debug.LogHelper;
import com.mediatek.camera.common.debug.LogUtil;
import com.mediatek.camera.common.mode.photo.heif.HeifWriter;
import com.mediatek.camera.common.storage.MediaSaver;
import com.mediatek.camera.common.utils.BitmapCreator;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.sql.Date;
import java.text.SimpleDateFormat;

/**
 * Created by mtk54274 on 7/13/18.
 */
public class HeifHelper {

    private static final LogUtil.Tag TAG = new LogUtil.Tag(HeifHelper.class.getSimpleName());
    public static int FORMAT_HEIF = ImageFormat.YUV_420_888;
    public static final long HEIF_WAIT_TIME = 10000;
    public static int orientation = -1;
    private static final String IMAGE_FORMAT = "'IMG'_yyyyMMdd_HHmmss_S";
    public static final String KEY_FORMAT = "key_format";
    public static final String SUFFIX_NAME = "heic";
    private static final String TEMP_SUFFIX = ".tmp";
    private HeifHelper.ImageFileName mImageFileName;
    private ICameraContext mICameraContext;

    public HeifHelper(ICameraContext cameraContext) {
        mICameraContext = cameraContext;
        mImageFileName = new HeifHelper.ImageFileName(IMAGE_FORMAT);
    }

    public Bitmap createBitmapFromYuv(byte[] yuvData, int yuvWidth, int yuvHeight, int
            targetWidth) {
        return BitmapCreator.createBitmapFromYuv(yuvData, FORMAT_HEIF, yuvWidth, yuvHeight,
                targetWidth, orientation);
    }

    public static int getCaptureFormat(String formatValue) {
        int format = ImageFormat.JPEG;
        if ("heif".equalsIgnoreCase(formatValue)) {
            format = HeifHelper.FORMAT_HEIF;
        }
        return format;
    }

    public static byte[] getYUVBuffer(Image image) {

        Image.Plane Y = image.getPlanes()[0];
        Image.Plane U = image.getPlanes()[1];
        Image.Plane V = image.getPlanes()[2];
        int Yb = Y.getBuffer().remaining();
        int Ub = U.getBuffer().remaining();
        int Vb = V.getBuffer().remaining();
        LogHelper.i(TAG, "[getBuffer] Yb = " + Yb
                + " Ub = " + Ub + " Vb = " + Vb);
        byte[] imageBuffer = new byte[Yb + Ub + Vb];

        Y.getBuffer().get(imageBuffer, 0, Yb);
        U.getBuffer().get(imageBuffer, Yb, Ub);
        V.getBuffer().get(imageBuffer, Yb + Ub, Vb);

        FileOutputStream out = null;
        try {
            // Write to a temporary file and rename it to the final name.
            // This
            // avoids other apps reading incomplete data.
            LogHelper.d(TAG, "save the data to SD Card");
            String outputPath = new File(Environment.getExternalStorageDirectory(),
                    "yuv")
                    .getAbsolutePath();
            out = new FileOutputStream(outputPath);
            out.write(imageBuffer);
            out.close();
        } catch (IOException e) {
            LogHelper.e(TAG, "Failed to write image,ex:", e);
        } finally {
            if (out != null) {
                try {
                    out.close();
                } catch (IOException e) {
                    LogHelper.e(TAG, "IOException:", e);
                }
            }
        }
        return imageBuffer;
    }

    public ContentValues getContentValues(int width, int height) {
        long dateTaken = System.currentTimeMillis();
        String fileDirectory = mICameraContext.getStorageService().getFileDirectory();
        String title = mImageFileName.generateTitle(dateTaken);
        //String filePath = fileDirectory + "/" + title + "." + SUFFIX_NAME;
        ContentValues contentValues = createContentValues(orientation, fileDirectory, dateTaken,
                width, height);
        return contentValues;
    }

    public static void saveData(byte[] data, int width, int height, int orientation, String
            filePath) {
        String tempFile = filePath + TEMP_SUFFIX;
        HeifWriter.Builder builder = new HeifWriter.Builder(
                filePath, width, height, HeifWriter.INPUT_MODE_BUFFER);
        builder.setGridEnabled(true);
        builder.setRotation(orientation);
        try {
            HeifWriter heifWriter = builder.build();
            heifWriter.start();
            heifWriter.addYuvBuffer(FORMAT_HEIF, data);
            try {
                long beginTime = System.currentTimeMillis();
                heifWriter.stop(HEIF_WAIT_TIME);
                LogHelper.i(TAG, "[saveData] save heif file consume time = " + (System
                        .currentTimeMillis() - beginTime));
                new File(tempFile).renameTo(new File(filePath));
            } catch (Exception e) {
                LogHelper.e(TAG, "Exception", e);
            }
            heifWriter.close();
        } catch (IOException e) {
            LogHelper.e(TAG, "getjpeg IOException ", e);
        }
    }

    /**
     * create a content values from data.
     *
     * @param orientation   the orientation of file.
     * @param fileDirectory file directory.
     * @param dateTaken taken time of the file
     * @param pictureWidth  the width of content values.
     * @param pictureHeight the height of content valuse.
     * @return the content values from the data.
     */
    public ContentValues createContentValues(int orientation, String fileDirectory,
                                             long dateTaken, int pictureWidth, int pictureHeight) {
        ContentValues values = new ContentValues();

        String title = mImageFileName.generateTitle(dateTaken);
        String mimetype = "image"+"/"+SUFFIX_NAME;
        String fileName = title +'.' + SUFFIX_NAME;
        String path = fileDirectory + '/' + fileName;

        values.put(MediaStore.Images.ImageColumns.DATE_TAKEN, dateTaken);
        values.put(MediaStore.Images.ImageColumns.TITLE, title);
        values.put(MediaStore.Images.ImageColumns.DISPLAY_NAME, fileName);
        values.put(MediaStore.Images.ImageColumns.MIME_TYPE, mimetype);
        values.put(MediaStore.Images.ImageColumns.WIDTH, pictureWidth);
        values.put(MediaStore.Images.ImageColumns.HEIGHT, pictureHeight);

        values.put(MediaStore.Images.ImageColumns.ORIENTATION, orientation);
        values.put(MediaStore.Images.ImageColumns.DATA, path);

        LogHelper.d(TAG, "createContentValues, width : " + pictureWidth + ",height = " +
                pictureHeight + ",orientation = " + orientation);
        return values;
    }

    /**
     * Used for create image file name.
     */
    private class ImageFileName {
        private SimpleDateFormat mSimpleDateFormat;

        public ImageFileName(String format) {
            mSimpleDateFormat = new SimpleDateFormat(format);
        }

        public String generateTitle(long dateTaken) {
            Date date = new Date(dateTaken);
            String result = mSimpleDateFormat.format(date);
            return result;
        }
    }
}
