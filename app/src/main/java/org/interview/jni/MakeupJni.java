package org.interview.jni;

import android.graphics.Bitmap;

public class MakeupJni {

    public native static void init();
    public native static void makeup(Bitmap inBmp, Bitmap outBmp, int type);
    public native static void destroy();
}
