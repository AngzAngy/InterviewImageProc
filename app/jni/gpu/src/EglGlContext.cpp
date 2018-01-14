#include "EglGlContext.h"
#include "jnilogger.h"

static void checkEglErr(const char *op){
    EGLint err = eglGetError();
    if(err != EGL_SUCCESS){
        if(op){
            LOGE("eglErr(%d) after %s", err, op);
        }else{
            LOGE("eglErr(%d)", err);
        }
    }
}

EglGlContext::EglGlContext()
:mEglDisplay(EGL_NO_DISPLAY),mEglContext(EGL_NO_CONTEXT),mEglSurface(EGL_NO_SURFACE){
}

EglGlContext::~EglGlContext() {
}

int EglGlContext::createEGLPbufferContext(){
    int ret = EGL_TRUE;
    EGLint majorVersion, minorVersion;
    EGLint dspConfigAttribs[]={
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_NONE
        };
    EGLConfig myConfig = {0};
    EGLint numConfig = 0;
    mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    checkEglErr("eglGetDisplay");
    if(EGL_NO_DISPLAY == mEglDisplay){
        LOGE("EGL Error NO Display");
        return -1;
    }
    ret = eglInitialize(mEglDisplay, &majorVersion, &minorVersion);
    checkEglErr("eglInitialize");
    LOGD("EGL majorVersion : %d,, minorVersion : %d---->", majorVersion, minorVersion);
    if(EGL_TRUE != ret){
        LOGE("eglInitialize failed ");
        return -1;
    }
    eglChooseConfig(mEglDisplay, dspConfigAttribs, &myConfig, 1, &numConfig);
    checkEglErr("eglChooseConfig");

    EGLint pbufferAttribs[]={
            EGL_WIDTH, 1,
            EGL_HEIGHT, 1,
            EGL_NONE
    };

    mEglSurface = eglCreatePbufferSurface(mEglDisplay, myConfig, pbufferAttribs);
    checkEglErr("eglCreatePbufferSurface");
    if(EGL_NO_SURFACE == mEglSurface){
        LOGE("EGL Error NO Surface");
        return -1;
    }

    EGLint contextAttribs[] ={EGL_CONTEXT_CLIENT_VERSION,2, EGL_NONE};
    mEglContext = eglCreateContext(mEglDisplay, myConfig, EGL_NO_CONTEXT, contextAttribs);
    checkEglErr("eglCreateContext");
    if(EGL_NO_CONTEXT == mEglContext){
        LOGE("EGL Error NO Context");
        return -1;
    }

    ret = eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext);
    checkEglErr("eglMakeCurrent");
    if(EGL_TRUE != ret){
        LOGE("EGL MakeCurrent failed ");
        return -1;
    }

    EGLint PbufW = 0, PbufH = 0;
    eglQuerySurface(mEglDisplay, mEglSurface, EGL_WIDTH, &PbufW);
    checkEglErr("eglQuerySurfaceW");
    eglQuerySurface(mEglDisplay, mEglSurface, EGL_HEIGHT, &PbufH);
    checkEglErr("eglQuerySurfaceH");
    LOGE("after eglQuerySurface w: %d h: %d\n", PbufW, PbufH);
    return ret;
}

int EglGlContext::destroyEGLContext(){
    if(EGL_NO_DISPLAY == mEglDisplay){
        return -1;
    }
    if(EGL_NO_CONTEXT != mEglContext){
        eglDestroyContext(mEglDisplay, mEglContext);
        mEglContext = EGL_NO_CONTEXT;
    }
    if(EGL_NO_SURFACE != mEglSurface){
        eglDestroySurface(mEglDisplay, mEglSurface);
        mEglSurface = EGL_NO_SURFACE;
    }
    if(EGL_NO_DISPLAY != mEglDisplay){
        eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglTerminate(mEglDisplay);
        mEglDisplay = EGL_NO_DISPLAY;
    }
    return 0;
}
