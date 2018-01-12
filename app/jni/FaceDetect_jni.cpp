#include "FaceDetect_jni.h"
#include "jRect.h"
#include "jnilogger.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/detection_based_tracker.hpp>

#include <string>
#include <vector>

#include <android/log.h>
#include <android/bitmap.h>


using namespace std;
using namespace cv;

static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
static CvSeq* objects = 0;
static double scale = 2.5;// 1.2;

int detect(IplImage* image);

JNIEXPORT jint JNICALL
Java_org_interview_jni_FaceDetectJni_detectFace
  (JNIEnv *env, jclass jthiz, jobject jbmp, jstring jfp){

    const char* cascade_name =env->GetStringUTFChars(jfp, NULL);// "haarcascade_frontalface_alt2.xml";
    cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0);
    env->ReleaseStringUTFChars(jfp, cascade_name);

    if (!cascade){
        LOGD("ERROR: Could not load classifier cascade\n");
        return 0;
    }
    storage = cvCreateMemStorage(0);

    int faceCount = 0;
    void *pixels;
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, jbmp, &info);
    AndroidBitmap_lockPixels(env, jbmp, &pixels);
    IplImage* image = cvCreateImageHeader(cvSize(info.width,info.height), 8, 4);
    if (image){
        image->imageData = (char *)pixels;
        faceCount = detect(image);
        cvReleaseImageHeader(&image);
    }
    AndroidBitmap_unlockPixels(env, jbmp);

    return faceCount;
}


JNIEXPORT void JNICALL
Java_org_interview_jni_FaceDetectJni_getFaceRects
  (JNIEnv *env, jclass jthiz, jobjectArray jrectArray){
    //Loop through found objects
    for (int i = 0; i<(objects ? objects->total : 0); ++i){
        jobject rectObj = env->GetObjectArrayElement(jrectArray, i);
        CvRect* r = (CvRect*)cvGetSeqElem(objects, i);
        if(rectObj){
            jRect rect(env, rectObj);
            rect.setLeft(int(r->x*scale));
            rect.setTop(int(r->y*scale));
            rect.setRight(int((r->x + r->width)*scale));
            rect.setBottom(int((r->y + r->height)*scale));
            LOGD("face%d,,x: %d;y: %d;rgt: %d;btm: %d",
                    i, int(r->x*scale), int(r->y*scale),
                    int((r->x + r->width)*scale), int((r->y + r->height)*scale));
            env->DeleteLocalRef(rectObj);
        }
//        cvRectangle(img, cvPoint(r->x*scale, r->y*scale), cvPoint((r->x + r->width)*scale, (r->y + r->height)*scale), colors[i % 8]);
    }

    objects = 0;
}

int detect(IplImage* img){
    //Image Preparation

    IplImage* gray = cvCreateImage(cvSize(img->width, img->height), 8, 1);
    IplImage* small_img = cvCreateImage(cvSize(cvRound(img->width / scale), cvRound(img->height / scale)), 8, 1);
    cvCvtColor(img, gray, CV_RGBA2GRAY);
    cvResize(gray, small_img, CV_INTER_LINEAR);

    cvEqualizeHist(small_img, small_img); //Ö±·½Í¼¾ùºâ

    //Detect objects if any
    cvClearMemStorage(storage);

    objects = cvHaarDetectObjects(small_img,
        cascade,
        storage,
        1.1,
        2,
        0,//CV_HAAR_DO_CANNY_PRUNING
        cvSize(30, 30));

    /*    //Loop through found objects and draw boxes around them
    for (int i = 0; i<(objects ? objects->total : 0); ++i)
    {
        CvRect* r = (CvRect*)cvGetSeqElem(objects, i);
//        cvRectangle(img, cvPoint(r->x*scale, r->y*scale), cvPoint((r->x + r->width)*scale, (r->y + r->height)*scale), colors[i % 8]);
    }
    for (int i = 0; i < (objects ? objects->total : 0); i++)
    {
        CvRect* r = (CvRect*)cvGetSeqElem(objects, i);
        CvPoint center;
        int radius;
        center.x = cvRound((r->x + r->width*0.5)*scale);
        center.y = cvRound((r->y + r->height*0.5)*scale);
        radius = cvRound((r->width + r->height)*0.25*scale);
        cvCircle(img, center, radius, colors[i % 8], 3, 8, 0);
    }*/

    cvReleaseImage(&gray);
    cvReleaseImage(&small_img);

    int count  = objects ? objects->total : 0;
    return count;
}

