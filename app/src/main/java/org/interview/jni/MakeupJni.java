package org.interview.jni;

import android.graphics.Bitmap;

public class MakeupJni {

    public native static void init();
    public native static void faceClean(Bitmap inBmp, Bitmap outBmp, float level);
    public native static void destroy();
}
