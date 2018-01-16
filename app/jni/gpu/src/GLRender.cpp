#include "GLRender.h"
#include "SelfDef.h"
#include <stdlib.h>

GLRender::GLRender():mProgram(NULL){
    memset(mTextures, 0, sizeof(GLTexture2d *)*MAX_TEXTURE_NUM);
}

GLRender::~GLRender(){
    destroyGLProgram();
    for(int i=0;i<MAX_TEXTURE_NUM;i++){
        deleteC(mTextures[i]);
    }
}

void GLRender::createGLProgram(const char * vertexShader, const char * fragShaderSrc){
    deleteC(mProgram);
    mProgram = new GLProgram(vertexShader, fragShaderSrc);
}

void GLRender::destroyGLProgram(){
    deleteC(mProgram);
}

void GLRender::createTexture(const GLvoid* pixels, int w, int h, GLint format, int textureUnit){
    int index = textureUnit - GL_TEXTURE0;
    deleteC(mTextures[index]);
    mTextures[index]= new GLTexture2d(pixels, w, h, format, textureUnit, GL_UNSIGNED_BYTE);
}

void GLRender::render(void* userData){
    if(!mProgram){
        return;
    }
    GLuint program = mProgram->getProgramId();
    mProgram->useProgram();

//    perRender();

//    GLint PositionAttribute = glGetAttribLocation(program, "aPosition");
//    GLint TextureCoordinateAttribute = glGetAttribLocation(program, "aTextureCoord");
//
//    glClearColor(0, 0, 0, 1);
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    glEnableVertexAttribArray(PositionAttribute);
//    glVertexAttribPointer(PositionAttribute, 2, GL_FLOAT, 0, 0, imageVertices);
//    glEnableVertexAttribArray(TextureCoordinateAttribute);
//    glVertexAttribPointer(TextureCoordinateAttribute, 2, GL_FLOAT, 0, 0, TextureCoordinates);
//
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//    postRender();
}

