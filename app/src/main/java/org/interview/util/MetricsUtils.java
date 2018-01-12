package org.interview.util;

import android.app.Activity;
import android.util.DisplayMetrics;

public abstract class MetricsUtils {
    private static float mPixelDensity = 1;
    private static int mPixelHeight = -1;
    private static int mPixelWidth = -1;
    private static int mDensityDpi = 1;

    private MetricsUtils() {
    }

    public static void initialize(Activity activity) {
        DisplayMetrics metrics = new DisplayMetrics();
        activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);
        mPixelDensity = metrics.density;
        mPixelHeight = metrics.heightPixels;
        mPixelWidth = metrics.widthPixels;
        mDensityDpi = metrics.densityDpi;
    }

    public static int dpToPixel(int dp) {
        return Math.round(mPixelDensity * dp);
    }

    public static final int screenWidth() {
        return mPixelWidth;
    }

    public static final int screenHeight() {
        return mPixelHeight;
    }

    public static final float screenDensity() {
        return mPixelDensity;
    }

    public static final int screenDensityDPI() {
        return mDensityDpi;
    }

}
