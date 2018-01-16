#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <stdint.h>
#include "GLProgram.h"
#include "GLTexture2d.h"

#ifndef _Included_GLRender
#define _Included_GLRender

#define MAX_TEXTURE_NUM 3

class GLRender {

public:

    GLRender();
    virtual ~GLRender();

    void createGLProgram(const char * vertexShader, const char * fragShaderSrc);
    void destroyGLProgram();
    void createTexture(const GLvoid* pixels, int w, int h, GLint format, int textureUnit);
    virtual void render(void* userData);

protected:
    GLProgram *mProgram;
    GLTexture2d *mTextures[MAX_TEXTURE_NUM];
};

#endif
