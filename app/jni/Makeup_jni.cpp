#include "Makeup_jni.h"
#include "jnilogger.h"
#include "MakeupRender.h"
#include "MakeupParams.h"
#include "SelfDef.h"
#include "EglGlContext.h"
#include <android/log.h>
#include <android/bitmap.h>

//static EglGlContext gCtxt;

static MakeupRender *gRender = NULL;

JNIEXPORT void JNICALL
Java_org_interview_jni_MakeupJni_init
  (JNIEnv *env, jclass jthiz){
//    gCtxt.createEGLPbufferContext();
    deleteC(gRender);
    gRender=new MakeupRender;
}

JNIEXPORT void JNICALL
Java_org_interview_jni_MakeupJni_makeup
  (JNIEnv *env, jclass jthiz, jobject jinbmp, jobject joutbmp, jint jtype){
    void *inpixels;
    void *outpixels;
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, jinbmp, &info);
    AndroidBitmap_lockPixels(env, jinbmp, &inpixels);
    AndroidBitmap_lockPixels(env, joutbmp, &outpixels);

    MakeupParams params;
    Image img;
    img.width = info.width;
    img.height = info.height;
    img.plane[0] = inpixels;
    img.plane[1] = outpixels;
    img.level = 0;
    params.pImg = &img;
    params.type = (MakeupType)jtype;

    if(gRender)
        gRender->render((void*)&params);

    AndroidBitmap_unlockPixels(env, joutbmp);
    AndroidBitmap_unlockPixels(env, jinbmp);
}

JNIEXPORT void JNICALL
Java_org_interview_jni_MakeupJni_destroy
  (JNIEnv *env, jclass jthiz){
    deleteC(gRender);
//    gCtxt.destroyEGLContext();
}

