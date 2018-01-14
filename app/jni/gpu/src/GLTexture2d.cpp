#include "GLTexture2d.h"
#include "jnilogger.h"
#include "GLUtil.h"

/*static int toolsavefile(unsigned char* pData, int size, char* name)
{
    FILE *pf =fopen(name, "wb");
    if(pf != 0){
        fwrite(pData, size, 1, pf);
        fclose(pf);
        return 1;

       }
    return 0;
}

static int toolloadfile(unsigned char* pData, int size, char* name)
{

    FILE *pf =fopen(name, "rb");
    if(pf != 0){
        fread(pData, size, 1, pf);
        fclose(pf);
        return 1;

       }
    return 0;
}*/

GLTexture2d::GLTexture2d(const GLvoid* pixels, int w, int h, GLint format, int textureUnit,GLenum type)
{
    genTexture(pixels, w, h, format, textureUnit, type);
}

GLTexture2d::GLTexture2d():mWidth(0),mHeight(0),mTextureUnit(0),mTextureId(0){

}

GLTexture2d::~GLTexture2d()
{
    glDeleteTextures(1, &mTextureId);
}

void GLTexture2d::genTexture(const GLvoid* pixels, int w, int h, GLint format, int textureUnit,GLenum type){
    mWidth = w;
    mHeight = h;
    mTextureUnit = textureUnit;

    glGenTextures(1, &mTextureId);
    GLUtil::checkGlError("glGenTextures");
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    GLUtil::checkGlError("glBindTexture");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLUtil::checkGlError("glTexParameteri filter");
    // This is necessary for non-power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLUtil::checkGlError("glTexParameteri wap");

     glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, type, pixels);
     GLUtil::checkGlError("glTexImage2D");
     glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture2d::subImage(const GLvoid* pixels,GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type){
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    GLUtil::checkGlError("subImage glBindTexture");
    glTexSubImage2D (GL_TEXTURE_2D, 0, xoffset,  yoffset, width, height, format, type, pixels);
    GLUtil::checkGlError("glTexSubImage2D");
    glBindTexture(GL_TEXTURE_2D, 0);
}

int GLTexture2d::getWidth(){
    return mWidth;
}

int GLTexture2d::getHeight(){
    return mHeight;
}

int GLTexture2d::getTextureUnit(){
    return mTextureUnit;
}

GLuint GLTexture2d::getTextureId(){
    return mTextureId;
}
