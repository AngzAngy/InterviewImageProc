package org.interview.util;

import java.io.Closeable;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.ActivityNotFoundException;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.YuvImage;
import android.net.Uri;
import android.os.Handler;
import android.os.ParcelFileDescriptor;
import android.util.Log;

public class Util {

    private static final String TAG = "Util";

    public static void Assert(boolean cond) {
        if (!cond) {
            throw new AssertionError();
        }
    }

    public static void closeSilently(Closeable c) {
        if (c == null)
            return;
        try {
            c.close();
        } catch (Throwable t) {
            // do nothing
        }
    }

    public static void joinThreadSilently(Thread t) {
        if (t == null)
            return;
        try {
            t.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public static void closeSilently(ParcelFileDescriptor c) {
        if (c == null)
            return;
        try {
            c.close();
        } catch (Throwable t) {
            // do nothing
        }
    }

    public static void dumpToFile(byte[] data, String path) {
        FileOutputStream os = null;
        try {
            os = new FileOutputStream(path);
            os.write(data);
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            closeSilently(os);
        }
    }

    public static void dumpNv21ToJpeg(byte[] nv21, int width, int height,
            String path) {
        FileOutputStream os = null;
        try {
            os = new FileOutputStream(path);
            Rect rect = new Rect(0, 0, width, height);
            YuvImage img = new YuvImage(nv21, ImageFormat.NV21, width, height,
                    null);
            img.compressToJpeg(rect, 85, os);
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            closeSilently(os);
        }
    }

    public static boolean isUriValid(Uri uri, ContentResolver resolver) {
        if (uri == null)
            return false;

        try {
            ParcelFileDescriptor pfd = resolver.openFileDescriptor(uri, "r");
            if (pfd == null) {
                Log.e(TAG, "Fail to open URI. URI=" + uri);
                return false;
            }
            pfd.close();
        } catch (IOException ex) {
            return false;
        }
        return true;
    }

    public static void viewUri(Uri uri, Context context) {
        if (!isUriValid(uri, context.getContentResolver())) {
            Log.e(TAG, "Uri invalid. uri=" + uri);
            return;
        }

        Intent intent = new Intent();
        intent.setData(uri);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        try {
            intent.setAction("com.android.action.REVIEW");
            context.startActivity(intent);
        } catch (ActivityNotFoundException ex) {
            try {
                intent.setAction(Intent.ACTION_VIEW);
                context.startActivity(intent);
            } catch (ActivityNotFoundException e) {
                Log.e(TAG, "review image fail. uri=" + uri, e);
            }
        }
    }

    public static Rect RectFtoRect(RectF r) {
        return new Rect((int) r.left, (int) r.top, (int) r.right,
                (int) r.bottom);
    }

    private static class BackgroundJob implements Runnable {

        private final Dialog mDialog;
        private final Runnable mJob;
        private final Handler mHandler;
        private final Runnable mCleanupRunner = new Runnable() {
            public void run() {
                if (mDialog.getWindow() != null)
                    mDialog.dismiss();
            }
        };

        public BackgroundJob(Activity activity, Runnable job, Dialog dialog,
                Handler handler) {
            mDialog = dialog;
            mJob = job;
            mHandler = handler;
        }

        public void run() {
            try {
                mJob.run();
            } finally {
                mHandler.post(mCleanupRunner);
            }
        }
    }

    public static void startBackgroundJob(Activity activity, String title,
            String message, Runnable job, Handler handler) {
        // Make the progress dialog uncancelable, so that we can gurantee
        // the thread will be done before the activity getting destroyed.
        ProgressDialog dialog = ProgressDialog.show(activity, title, message,
                true, false);
        new Thread(new BackgroundJob(activity, job, dialog, handler)).start();
    }

}
