package org.interview.facedetect;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Paint.Style;
import android.util.AttributeSet;
import android.widget.ImageView;

public class FaceImageView extends ImageView {

    private Rect[] mRectFaces = null;
    private Paint mPntFace = null;
    public FaceImageView(Context context) {
        super(context);
        initialize();
    }

    public FaceImageView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize();
    }

    public FaceImageView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        initialize();
    }

    private void initialize() {
        mPntFace = new Paint();
        mPntFace.setColor(Color.GREEN);
        mPntFace.setStyle(Style.STROKE);
    }

    public void setFaces(Rect[] faces) {
        mRectFaces = faces;
        postInvalidate();
    }

    public void onDraw(Canvas canvas) {
        try {
            super.onDraw(canvas);
            if (mRectFaces != null) {
                for (Rect rect : mRectFaces) {
                    canvas.drawRect(rect, mPntFace);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
