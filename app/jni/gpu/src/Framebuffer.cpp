#include "Framebuffer.h"
#include "GLUtil.h"

Framebuffer::Framebuffer():mframebufferId(0){};

void Framebuffer::createFBO(){
    glGenFramebuffers(1, &mframebufferId);
    GLUtil::checkGlError("glGenFramebuffers");
}

void Framebuffer::bindTexture(GLenum textureTarget, GLuint textureId){
    glBindFramebuffer(GL_FRAMEBUFFER, mframebufferId);
    GLUtil::checkGlError("glBindFramebuffer");

//    glActiveTexture(texture);
//    glBindTexture(mTextureTarget, mFboTextureId);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, textureId, 0);
    GLUtil::checkGlError("glFramebufferTexture2D");
}

void Framebuffer::unbind(GLenum textureTarget){
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer(){
    glDeleteFramebuffers(1, &mframebufferId);
    GLUtil::checkGlError("glDeleteFramebuffers");
};


