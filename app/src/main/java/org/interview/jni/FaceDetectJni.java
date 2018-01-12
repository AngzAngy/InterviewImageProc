package org.interview.jni;

import android.graphics.Bitmap;
import android.graphics.Rect;

public class FaceDetectJni {


    public native static int detectFace(Bitmap bmp, String cascadeFile);
    public native static void getFaceRects(Rect[] rects);
}
