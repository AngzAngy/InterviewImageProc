#ifndef __EGL_GL_CONTEXT_H__
#define __EGL_GL_CONTEXT_H__
#include <EGL/egl.h>

class EglGlContext{
    public:

        EglGlContext();
        ~EglGlContext();

        int createEGLPbufferContext();
        int destroyEGLContext();

        EGLContext getEglContext(){
            return mEglContext;
        }

        EGLDisplay getEglDisplay(){
            return mEglDisplay;
        }

    private:

        EGLDisplay mEglDisplay;
        EGLContext mEglContext;
        EGLSurface mEglSurface;
};

#endif // end __EGL_GL_CONTEXT_H__
