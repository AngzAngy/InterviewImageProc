package org.interview.facedetect;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import org.interview.util.StorageUtil;
import org.interview.util.ToastUtil;


import android.app.Activity;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.provider.MediaStore.Images;
import android.view.View;
import android.view.View.OnClickListener;


public class MainPageActivity extends Activity implements OnClickListener {

    private static final int PHOTO_PICKED_WITH_NAME = 1;
    private static final int CAMERA_CAPTURE_WITH_NAME = 2;

    private Context mContext = null;

    private Uri mUri = null;
    private String mPath = null;
    private boolean mIsClick = false;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mContext = getApplicationContext();
        setContentView(R.layout.main);
        // if(!checkDeviceID()) {
        // ToastUtil.showShortToast(mContext, "Invalid IMEI!");
        // finish();
        // return;
        // }

        findViewById(R.id.camera_logo).setOnClickListener(this);
        findViewById(R.id.photo_logo).setOnClickListener(this);
        findViewById(R.id.bnsample1).setOnClickListener(this);
        findViewById(R.id.bnsample2).setOnClickListener(this);
        findViewById(R.id.bnsample3).setOnClickListener(this);
        findViewById(R.id.bnsample4).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        if (mIsClick) {
            return;
        }
        mIsClick = true;
        switch (v.getId()) {
        case R.id.camera_logo:
            jumpToCamera();
            break;
        case R.id.photo_logo:
            jumpToGallery();
            break;
        case R.id.bnsample1:
            String path = getAssetsFilePath("sample1.jpg");
            jumpToEditor(path);
            break;
        case R.id.bnsample2:
            path = getAssetsFilePath("sample2.jpg");
            jumpToEditor(path);
            break;
        case R.id.bnsample3:
            path = getAssetsFilePath("sample3.jpg");
            jumpToEditor(path);
            break;
        case R.id.bnsample4:
            path = getAssetsFilePath("sample4.jpg");
            jumpToEditor(path);
            break;
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        mIsClick = false;
    }

    private String getAssetsFilePath(String assets) {
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

    private void jumpToCamera() {
        Intent inttPhoto = new Intent("android.media.action.IMAGE_CAPTURE");
        long dateTaken = System.currentTimeMillis();
        String name = String.format("%d", dateTaken) + ".jpg";
        String filename = StorageUtil.DIRECTORY + "/" + name;

        ContentValues values = new ContentValues();
        values.put(Images.Media.TITLE, filename);
        values.put("_data", filename);
        values.put(Images.Media.PICASA_ID, filename);
        values.put(Images.Media.DISPLAY_NAME, filename);
        values.put(Images.Media.DESCRIPTION, filename);
        values.put(Images.ImageColumns.BUCKET_DISPLAY_NAME, filename);
        mUri = getContentResolver().insert(
                MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
        inttPhoto.putExtra(MediaStore.EXTRA_OUTPUT, mUri);
        mPath = filename;
        startActivityForResult(inttPhoto, CAMERA_CAPTURE_WITH_NAME);
    }

    private void jumpToGallery() {
        // if (bButtonDoing == true)
        // return;
        // bButtonDoing = true;

        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("image/*");

        startActivityForResult(intent, PHOTO_PICKED_WITH_NAME);
    }

    private void jumpToEditor(String path) {
        if (!StorageUtil.checkStorage()) {
            ToastUtil.showShortToast(getApplicationContext(),
                    R.string.common_no_storage);
            return;
        }
        Intent intent = new Intent(mContext, FaceDetectActivity.class);
        intent.putExtra(FaceDetectActivity.INTENT_EXTRA_PATH, path);
        startActivity(intent);
    }

    private void jumpToEditor(Uri uri) {
        if (!StorageUtil.checkStorage()) {
            ToastUtil.showShortToast(getApplicationContext(),
                    R.string.common_no_storage);
            return;
        }
        Intent intent = new Intent(mContext, FaceDetectActivity.class);
        intent.setData(uri);
        startActivity(intent);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode != RESULT_OK) {
            return;
        }
        switch (requestCode) {
        case PHOTO_PICKED_WITH_NAME:
            if (data != null) {
                Uri uri = data.getData();
                jumpToEditor(uri);
            }
            break;
        case CAMERA_CAPTURE_WITH_NAME:
            jumpToEditor(mPath);
            break;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

    }

}
