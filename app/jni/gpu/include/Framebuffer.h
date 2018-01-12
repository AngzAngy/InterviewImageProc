#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifndef _Included_FRAMEBUFFER__H__
#define _Included_FRAMEBUFFER__H__

class Framebuffer {
public:

    Framebuffer();
    ~Framebuffer();
    void createFBO();
    void bindTexture(GLenum textureTarget,GLuint textureId);
    void unbind(GLenum textureTarget);
    GLuint getFrameBufferId(){
        return mframebufferId;
    }

private:
    GLuint mframebufferId;

};
#endif
