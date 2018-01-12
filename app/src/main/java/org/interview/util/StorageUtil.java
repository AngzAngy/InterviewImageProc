

package org.interview.util;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.location.Location;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.Environment;
import android.os.StatFs;
import android.provider.MediaStore.Images;
import android.provider.MediaStore.Images.ImageColumns;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class StorageUtil {
    private static final String TAG = "CameraStorage";

    private static final String DCIM = Environment
            .getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM)
            .toString();
    public static final String DIRECTORY = DCIM + "/Camera";

    // Match the code in MediaProvider.computeBucketValues().
    public static final String BUCKET_ID = String.valueOf(DIRECTORY
            .toLowerCase().hashCode());

    public static final long UNAVAILABLE = -1L;
    public static final long PREPARING = -2L;
    public static final long UNKNOWN_SIZE = -3L;
    public static final long LOW_STORAGE_THRESHOLD = 5000000; // In bytes
    public static final long PICTURE_SIZE = 1500000;

    private static final int BUFSIZE = 4096;

    public static Uri addImage(ContentResolver resolver, String path,
            long date, Location location, int orientation, byte[] jpeg,
            int width, int height) {
        // Save the image.
        if (!saveImageToStorage(path, jpeg)) {
            return null;
        }
        return insertImageToMediaStore(path, date, orientation, jpeg.length,
                location, resolver);
    }

    public static Uri addImage(ContentResolver resolver, String path,
            long date, Location location, int orientation, Bitmap bmp) {
        // Save the image.
        if (!saveImageToStorage(path, bmp)) {
            return null;
        }
        if (orientation != 0) {
            try {
                ExifInterface exif = new ExifInterface(path);
                exif.setAttribute(ExifInterface.TAG_ORIENTATION,
                        String.valueOf(orientation));
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return insertImageToMediaStore(path, date, orientation, 0, location,
                resolver);
    }

    public static boolean saveImageToStorage(String path, Bitmap bmp) {
        FileOutputStream out = null;
        try {
            out = new FileOutputStream(path);
            bmp.compress(CompressFormat.JPEG, 90, out);
        } catch (Exception e) {
            Log.e(TAG, "Failed to write image", e);
            return false;
        } finally {
            try {
                out.close();
            } catch (Exception e) {
            }
        }
        return true;
    }

    private static boolean saveImageToStorage(String path, byte[] jpeg) {
        FileOutputStream out = null;
        try {
            out = new FileOutputStream(path);
            out.write(jpeg);
        } catch (Exception e) {
            Log.e(TAG, "Failed to write image", e);
            return false;
        } finally {
            try {
                out.close();
            } catch (Exception e) {
            }
        }
        return true;
    }

    private static Uri insertImageToMediaStore(String path, long date,
            int orientation, long size, Location location,
            ContentResolver resolver) {
        int tileBegin = path.lastIndexOf("/");
        int tileend = path.lastIndexOf(".");
        String title = path.substring(tileBegin, tileend);
        // Insert into MediaStore.
        ContentValues values = new ContentValues(9);
        values.put(ImageColumns.TITLE, title);
        values.put(ImageColumns.DISPLAY_NAME, title + ".jpg");
        values.put(ImageColumns.DATE_TAKEN, date);
        values.put(ImageColumns.MIME_TYPE, "image/jpeg");
        values.put(ImageColumns.ORIENTATION, orientation);
        values.put(ImageColumns.DATA, path);
        values.put(ImageColumns.SIZE, size);
        // values.put(ImageColumns.WIDTH, width);
        // values.put(ImageColumns.HEIGHT, height);

        if (location != null) {
            values.put(ImageColumns.LATITUDE, location.getLatitude());
            values.put(ImageColumns.LONGITUDE, location.getLongitude());
        }

        Uri uri = resolver.insert(Images.Media.EXTERNAL_CONTENT_URI, values);
        if (uri == null) {
            Log.e(TAG, "Failed to write MediaStore");
            return null;
        }
        return uri;
    }

    public static long getAvailableSpace() {
        String state = Environment.getExternalStorageState();
        Log.d(TAG, "External storage state=" + state);
        if (Environment.MEDIA_CHECKING.equals(state)) {
            return PREPARING;
        }
        if (!Environment.MEDIA_MOUNTED.equals(state)) {
            return UNAVAILABLE;
        }

        File dir = new File(DIRECTORY);
        dir.mkdirs();
        if (!dir.isDirectory() || !dir.canWrite()) {
            return UNAVAILABLE;
        }

        try {
            StatFs stat = new StatFs(DIRECTORY);
            return stat.getAvailableBlocks() * (long) stat.getBlockSize();
        } catch (Exception e) {
            Log.i(TAG, "Fail to access external storage", e);
        }
        return UNKNOWN_SIZE;
    }

    /**
     * OSX requires plugged-in USB storage to have path /DCIM/NNNAAAAA to be
     * imported. This is a temporary fix for bug#1655552.
     */
    public static void ensureOSXCompatible() {
        File nnnAAAAA = new File(DIRECTORY);
        if (!(nnnAAAAA.exists() || nnnAAAAA.mkdirs())) {
            Log.e(TAG, "Failed to create " + nnnAAAAA.getPath());
        }
    }

    public static boolean checkStorage() {
        return getAvailableSpace() > LOW_STORAGE_THRESHOLD;
    }
}
