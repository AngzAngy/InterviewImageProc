#include "MakeupRender.h"
#include "Image.h"
#include "jnilogger.h"
#include "SelfDef.h"
#include <stdlib.h>

static const GLfloat imageVertices[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f,  1.0f,
};

//static const GLfloat TextureCoordinates[] = {
//    0.0f, 1.0f,
//    1.0f, 1.0f,
//    0.0f, 0.0f,
//    1.0f, 0.0f,
//};

static const GLfloat TextureCoordinates[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
};

MakeupRender::MakeupRender():mOutTexture(NULL),mInTexture(NULL){
    mCtxt.createEGLPbufferContext();
}

MakeupRender::~MakeupRender(){
    deleteC(mInTexture);
    deleteC(mOutTexture);
    mCtxt.destroyEGLContext();
}

void MakeupRender::setup(int width, int height){
    if(NULL==mProgram){
        char vShader[] = "attribute vec4 aPosition;"
                "attribute vec2 aTextureCoord;"
                "varying vec2 vTextureCoord;"
                "void main() {"
                " gl_Position = aPosition; "
                "vTextureCoord = aTextureCoord;}";

        char fShader[] = "precision mediump float;"
    "varying vec2 vTextureCoord;"
    "uniform sampler2D sTexture;"
    "uniform vec2 imageSize;    "
    "uniform float level;       "
    "void main() {              "
    "vec4 sum;                  "
    "vec2 offset0=vec2(-1.0,-1.0); vec2 offset1=vec2(0.0,-1.0); vec2 offset2=vec2(1.0,-1.0);"
    "vec2 offset3=vec2(-1.0,0.0); vec2 offset4=vec2(0.0,0.0); vec2 offset5=vec2(1.0,0.0);                        "
    "vec2 offset6=vec2(-1.0,1.0); vec2 offset7=vec2(0.0,1.0); vec2 offset8=vec2(1.0,1.0);                        "
    "float kernelValue0 = 0.999900; float kernelValue1 = 0.999950; float kernelValue2 = 0.999900;                "
    "float kernelValue3 = 0.999950; float kernelValue4 =1.000000; float kernelValue5 = 0.999950;                 "
    "float kernelValue6 = 0.999900; float kernelValue7 = 0.999950; float kernelValue8 = 0.999900;                "
    "vec4 cTemp0;vec4 cTemp1;vec4 cTemp2;vec4 cTemp3;vec4 cTemp4;vec4 cTemp5;vec4 cTemp6;vec4 cTemp7;vec4 cTemp8;"
    "vec2 offset_coor;                                     "
    "offset_coor=(offset0/imageSize)*level;                "
    "cTemp0=texture2D(sTexture, vTextureCoord+offset_coor);"
    "offset_coor=(offset1/imageSize)*level;                "
    "cTemp1=texture2D(sTexture, vTextureCoord+offset_coor);"
    "offset_coor=(offset2/imageSize)*level;                "
    "cTemp2=texture2D(sTexture, vTextureCoord+offset_coor);"
    "offset_coor=(offset3/imageSize)*level;                "
    "cTemp3=texture2D(sTexture, vTextureCoord+offset_coor);"
    "offset_coor=(offset4/imageSize)*level;                "
    "cTemp4=texture2D(sTexture, vTextureCoord+offset_coor);"
    "offset_coor=(offset5/imageSize)*level;                "
    "cTemp5=texture2D(sTexture, vTextureCoord+offset_coor);"
    "offset_coor=(offset6/imageSize)*level;                "
    "cTemp6=texture2D(sTexture, vTextureCoord+offset_coor);"
    "offset_coor=(offset7/imageSize)*level;                "
    "cTemp7=texture2D(sTexture, vTextureCoord+offset_coor);"
    "offset_coor=(offset8/imageSize)*level;                "
    "cTemp8=texture2D(sTexture, vTextureCoord+offset_coor);"
    "sum =kernelValue0*cTemp0+kernelValue1*cTemp1+kernelValue2*cTemp2+"
    "    kernelValue3*cTemp3+kernelValue4*cTemp4+kernelValue5*cTemp5+ "
    "    kernelValue6*cTemp6+kernelValue7*cTemp7+kernelValue8*cTemp8; "
    "float factor=kernelValue0+kernelValue1+kernelValue2+kernelValue3+kernelValue4+kernelValue5+kernelValue6+kernelValue7+kernelValue8;"
    "gl_FragColor = sum/factor;"
    "}";
        glViewport(0, 0, width, height);
        glEnable(GL_TEXTURE);
        createGLProgram(vShader, fShader);

        mInTexture=new GLTexture2d(NULL, width, height, GL_RGBA, GL_TEXTURE0);
        mFbo.createFBO();
        mOutTexture=new GLTexture2d(NULL, width, height, GL_RGBA, GL_TEXTURE1);
        LOGI("setup,,size: %d x %d", width, height);
    }
}
void MakeupRender::render(void* userData){
    Image *pImg = (Image *)userData;
    if(!pImg){
        return;
    }

    setup(pImg->width, pImg->height);
    LOGI("render,,size: %d x %d,,,level:%f", pImg->width, pImg->height, pImg->level);

    mProgram->useProgram();
    GLuint program = mProgram->getProgramId();

    glBindTexture(GL_TEXTURE_2D, mOutTexture->getTextureId());
    mFbo.bindTexture(GL_TEXTURE_2D, mOutTexture->getTextureId());

    float imageSize[2];
    imageSize[0]=pImg->width;
    imageSize[0]=pImg->height;

    mInTexture->subImage((const GLvoid*)(pImg->plane[0]), 0, 0, pImg->width, pImg->height, GL_RGBA);
    mProgram->bindTexture("sTexture", mInTexture->getTextureId(), GL_TEXTURE0);
    mProgram->setUniformv("imageSize", imageSize, 2);
    mProgram->setUniform1f("level", pImg->level);
    GLint PositionAttribute = glGetAttribLocation(program, "aPosition");
    GLint TextureCoordinateAttribute = glGetAttribLocation(program, "aTextureCoord");

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableVertexAttribArray(PositionAttribute);
    glVertexAttribPointer(PositionAttribute, 2, GL_FLOAT, 0, 0, imageVertices);
    glEnableVertexAttribArray(TextureCoordinateAttribute);
    glVertexAttribPointer(TextureCoordinateAttribute, 2, GL_FLOAT, 0, 0, TextureCoordinates);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glReadPixels(0, 0, (GLsizei)pImg->width, (GLsizei)pImg->height,
            GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)(pImg->plane[1]));

    mProgram->unbindTexture(GL_TEXTURE0);
    mFbo.unbind(GL_TEXTURE_2D);
    mProgram->unbindTexture(GL_TEXTURE1);
}

