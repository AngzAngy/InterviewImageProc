package org.interview.facedetect;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import org.interview.jni.FaceDetectJni;
import org.interview.jni.MakeupJni;
import org.interview.util.BitmapUtil;
import org.interview.util.MetricsUtils;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnKeyListener;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.Rect;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;


public class FaceDetectActivity extends Activity implements SeekBar.OnSeekBarChangeListener, View.OnTouchListener{
    public static final String INTENT_EXTRA_PATH = "path";
    private FaceImageView mFaceImageView;
    private Bitmap mFaceBmp;
    private Bitmap mFaceBmpBk;
    private String mCascadeFile;
    private MakeupThread mMakeupThread;
    private SeekBar mSeekBar;
    private int mCurrentProc;
    private TextView mOriginalActionView;

    /*
     * Load Library Must do this first
     */
    static {
        try {
            System.loadLibrary("face_detect_jni");
        } catch (UnsatisfiedLinkError e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        MetricsUtils.initialize(this);

        mCascadeFile = writeAssetsFilePath("haarcascade_frontalface_alt2.xml");

        setContentView(R.layout.face_detect_layout);
        mFaceImageView = (FaceImageView) this.findViewById(R.id.filter_img_v);
        mSeekBar = (SeekBar)findViewById(R.id.seekbar_id);
        mSeekBar.setOnSeekBarChangeListener(this);
        mOriginalActionView = ((TextView) findViewById(R.id.original_id));
        mOriginalActionView.setOnTouchListener(this);

        Intent intent = getIntent();
        Uri uri = intent.getData();
        String path = intent.getStringExtra(INTENT_EXTRA_PATH);

        mMakeupThread = new MakeupThread();
        mMakeupThread.startProcess();
        new LoadBitmapAsyncTask(uri,path).execute();
    }

    private String writeAssetsFilePath(String assets) {
        InputStream is = null;
        FileOutputStream fos = null;
        File file = new File(getFilesDir() + "/" + assets);
        if (file.exists()) {
            return file.getAbsolutePath();
        }
        try {
            is = getAssets().open(assets);
            fos = new FileOutputStream(file);
            byte[] buf = new byte[512];
            int bytesRead = 0;
            while ((bytesRead = is.read(buf)) > 0) {
                fos.write(buf, 0, bytesRead);
            }
            return file.getAbsolutePath();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (is != null)
                    is.close();
                if (fos != null)
                    fos.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return null;
    }

    protected void onDestroy() {
        super.onDestroy();
        mMakeupThread.stopProcess();
        mMakeupThread=null;
        BitmapUtil.recycleBitmap(mFaceBmp);
        BitmapUtil.recycleBitmap(mFaceBmpBk);
    }

    class LoadBitmapAsyncTask extends AsyncTask<Void, Void, Integer> {
        private Uri mUri;
        private String mPath;
        private ProgressDialog mProgressDialog;
        public LoadBitmapAsyncTask(Uri uri, String path) {
            super();
            mUri = uri;
            mPath = path;
        }

        @Override
        protected void onPreExecute() {
            mProgressDialog = new ProgressDialog(FaceDetectActivity.this);
            mProgressDialog.setMessage(getString(R.string.text_loading));
            mProgressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
            mProgressDialog.setCanceledOnTouchOutside(false);
            mProgressDialog.setOnKeyListener(new OnKeyListener() {
                @Override
                public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                    return true;
                }
            });
            if(!FaceDetectActivity.this.isFinishing()) {
                mProgressDialog.show();
            }
        }

        @Override
        protected Integer doInBackground(Void... params) {
            BitmapUtil.recycleBitmap(mFaceBmp);
            BitmapUtil.recycleBitmap(mFaceBmpBk);
            if (mUri != null) {
                mFaceBmp = BitmapUtil.getBitmap(mUri, getApplicationContext(),
                        MetricsUtils.screenWidth(), MetricsUtils.screenHeight());
            }
            if (mPath != null) {
                mFaceBmp = BitmapUtil.getBitmap(mPath,
                        MetricsUtils.screenWidth(), MetricsUtils.screenHeight());
            }
            mFaceBmpBk = mFaceBmp.copy(Bitmap.Config.ARGB_8888, true);
            int count = FaceDetectJni.detectFace(mFaceBmp, mCascadeFile);
            return count;
        }

        @Override
        protected void onPostExecute(Integer result) {
            mProgressDialog.dismiss();

            if(result>0){
                Rect []rectArr = new Rect[result];
                for(int i=0; i< rectArr.length;i++){
                    rectArr[i] = new Rect();
                }
                FaceDetectJni.getFaceRects(rectArr);
                mFaceImageView.setImageBitmap(mFaceBmp);
                mFaceImageView.setFaces(rectArr);
                mSeekBar.setVisibility(View.VISIBLE);
                mOriginalActionView.setVisibility(View.VISIBLE);
            }else{
                mFaceImageView.setImageBitmap(mFaceBmp);
                mFaceImageView.setFaces(null);
            }
        }
    }

    @Override
    public void onProgressChanged(SeekBar arg0, final int proc, boolean arg2) {
            mCurrentProc = proc;

            if(mMakeupThread!=null){
                mMakeupThread.pushQueue(new Runnable() {
                    @Override
                    public void run() {
                      float level = proc * 0.04f;
                      Log.e("MY_LOG_TAG", "level=="+level+",,,proc=="+mCurrentProc);
                      MakeupJni.faceClean(mFaceBmpBk, mFaceBmp, level);
                      mFaceImageView.post(new Runnable() {
                          @Override
                          public void run() {
                              mFaceImageView.setImageBitmap(mFaceBmp);
                          }
                      });
                    }
                });
            }
    }

    @Override
    public void onStartTrackingTouch(SeekBar arg0) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar arg0) {
    }

    @Override
    public boolean onTouch(View arg0, MotionEvent event) {
        switch(event.getAction()){
        case MotionEvent.ACTION_DOWN:
            mOriginalActionView.setTextColor(Color.GREEN);
            mFaceImageView.setImageBitmap(mFaceBmpBk);
            break;
        case MotionEvent.ACTION_UP:
            mOriginalActionView.setTextColor(Color.WHITE);
            mFaceImageView.setImageBitmap(mFaceBmp);
            break;
        }
        return true;
    }
}
