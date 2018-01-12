#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include "EglGlContext.h"
#include "GLRender.h"
#include "Framebuffer.h"

#ifndef _Included_MAKEUPRender
#define _Included_MAKEUPRender

class MakeupRender : public GLRender{

public:

    MakeupRender();
    virtual ~MakeupRender();

    virtual void render(void* userData);
private:
    void setup(int width, int height);
    EglGlContext mCtxt;
    GLTexture2d *mInTexture;
    Framebuffer mFbo;
    GLTexture2d *mOutTexture;
};

#endif
