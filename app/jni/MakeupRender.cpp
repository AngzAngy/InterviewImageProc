#include "MakeupRender.h"
#include "MakeupParams.h"
#include "jnilogger.h"
#include "SelfDef.h"
#include <stdlib.h>
#include "GLUtil.h"

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

static const char vShader[] = "attribute vec4 aPosition;"
                "attribute vec2 aTextureCoord;"
                "varying vec2 vTextureCoord;"
                "void main() {"
                " gl_Position = aPosition; "
                "vTextureCoord = aTextureCoord;}";

static const char bilateralFilterShader[] = "precision mediump float;"
    "varying vec2 vTextureCoord;"
    "uniform sampler2D sTexture;"
    "uniform vec2 imageSize;    "
    "uniform float level;       "
    "void main() {              "
    "float sigma_dis = 25.0;    "
    "float sigma_pixel = 1.5*15.0/255.0;"
    "float maskSum = 0.0;"
    "vec3 valueSum =vec3(0.0,0.0,0.0);"
    "float weight_dis[25];"
    "vec2 offset[25];"
    "weight_dis[0] = 0.9936;weight_dis[1] = 0.996;weight_dis[2] = 0.9968;weight_dis[3] = 0.996;weight_dis[4] = 0.9936;"
    "weight_dis[5] = 0.996;weight_dis[6] = 0.9984;weight_dis[7] = 0.9992;weight_dis[8] = 0.9984;weight_dis[9] = 0.996;"
    "weight_dis[10] = 0.9968;weight_dis[11] = 0.9992;weight_dis[12] = 1.0;weight_dis[13] = 0.9992;weight_dis[14] = 0.9968;"
    "weight_dis[15] = 0.996;weight_dis[16] = 0.9984;weight_dis[17] = 0.9992;weight_dis[18] = 0.9984;weight_dis[19] = 0.996;"
    "weight_dis[20] = 0.9936;weight_dis[21] = 0.996;weight_dis[22] = 0.9968;weight_dis[23] = 0.996;weight_dis[24] = 0.9936;"
    "offset[0]=vec2(-2.0,-2.0);offset[1]=vec2(-1.0,-2.0);offset[2]=vec2(0.0,-2.0);"
    "offset[3]=vec2(1.0,-2.0);offset[4]=vec2(2.0,-2.0);"
    "offset[5]=vec2(-2.0,-1.0);offset[6]=vec2(-1.0,-1.0);offset[7]=vec2(0.0,-1.0);"
    "offset[8]=vec2(1.0,-1.0);offset[9]=vec2(2.0,-1.0);"
    "offset[10]=vec2(-2.0,0.0);offset[11]=vec2(-1.0,0.0);offset[12]=vec2(0.0,0.0);"
    "offset[13]=vec2(1.0,0.0);offset[14]=vec2(2.0,0.0);"
    "offset[15]=vec2(-2.0,1.0);offset[16]=vec2(-1.0,1.0);offset[17]=vec2(0.0,1.0);"
    "offset[18]=vec2(1.0,1.0);offset[19]=vec2(2.0,1.0);"
    "offset[20]=vec2(-2.0,2.0);offset[21]=vec2(-1.0,2.0);offset[22]=vec2(0.0,2.0);"
    "offset[23]=vec2(1.0,2.0);offset[24]=vec2(2.0,2.0);"
    "vec4 curColor = texture2D(sTexture,vTextureCoord);"
    "vec4 color = vec4(0.0);vec3 diffColor = vec3(0.0);float maskColor=0.0;float mask=0.0;"
    "for(int i=0; i < 25; i++){"
        "color = texture2D(sTexture,vTextureCoord + vec2(offset[i].x/imageSize.x,offset[i].y/imageSize.y));"
        "diffColor = color.rgb - curColor.rgb;"
        "maskColor = exp(-dot(diffColor, diffColor)/(2.0*sigma_pixel*sigma_pixel));"
        "mask =  maskColor * weight_dis[i];"
        "valueSum = color.rgb * mask + valueSum;"
        "maskSum +=  mask;}"
    "gl_FragColor = vec4(valueSum/maskSum, 1.0);"
    "}";

static const char logCurveShader[] = "precision mediump float;"
        "varying vec2 vTextureCoord;"
        "uniform sampler2D sTexture;"
        "uniform vec2 imageSize;    "
        "uniform float level;       "
        "void main(){"
            "float beta = 2.0;"//beta为调节参数，绘制了beta分别为[2,3,4,5]时,beta越大，美白的程度越强。
            "vec4 curColor;"
            "vec3 curveColor;"
            "vec3 one = vec3(1.0);"
             "curColor= texture2D(sTexture,vTextureCoord);"
             "curveColor= (1.0 / log(beta)) * log((beta - 1.0) * curColor.rgb + one);"
            "gl_FragColor = vec4(curveColor, 1.0);"
        "}";

MakeupRender::MakeupRender():mOutTexture(NULL),mInTexture(NULL),bSetup(false){
    mCtxt.createEGLPbufferContext();
}

MakeupRender::~MakeupRender(){
    deleteC(mInTexture);
    deleteC(mOutTexture);
    mCtxt.destroyEGLContext();
}

void MakeupRender::setup(int width, int height){
    if(!bSetup){
        glViewport(0, 0, width, height);
        GLUtil::checkGlError("glViewport");
        GLUtil::checkGlError("glEnable(GL_TEXTURE)");

        mInTexture=new GLTexture2d(NULL, width, height, GL_RGBA, GL_TEXTURE0);
        mFbo.createFBO();
        mOutTexture=new GLTexture2d(NULL, width, height, GL_RGBA, GL_TEXTURE1);
        LOGI("setup,,size: %d x %d", width, height);
        bSetup = true;
    }
}
void MakeupRender::render(void* userData){
    MakeupParams *params = (MakeupParams *)userData;
    if(!params){
        return;
    }

    destroyGLProgram();
    switch(params->type){
        case SKINWHITEN:
            createGLProgram(vShader, logCurveShader);
            break;
        case FACECLEAN:
        default:
            createGLProgram(vShader, bilateralFilterShader);
            break;
    }

    setup(params->pImg->width, params->pImg->height);
    LOGI("render,,size: %d x %d,,,level:%f", params->pImg->width, params->pImg->height, params->pImg->level);

    mProgram->useProgram();
    GLuint program = mProgram->getProgramId();

    glBindTexture(GL_TEXTURE_2D, mOutTexture->getTextureId());
    mFbo.bindTexture(GL_TEXTURE_2D, mOutTexture->getTextureId());

    float imageSize[2];
    imageSize[0]=params->pImg->width;
    imageSize[1]=params->pImg->height;

    mInTexture->subImage((const GLvoid*)(params->pImg->plane[0]), 0, 0, params->pImg->width, params->pImg->height, GL_RGBA);
    mProgram->bindTexture("sTexture", mInTexture->getTextureId(), GL_TEXTURE0);
    mProgram->setUniformv("imageSize", imageSize, 2);
    mProgram->setUniform1f("level", params->pImg->level);
    GLint PositionAttribute = glGetAttribLocation(program, "aPosition");
    GLint TextureCoordinateAttribute = glGetAttribLocation(program, "aTextureCoord");

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableVertexAttribArray(PositionAttribute);
    glVertexAttribPointer(PositionAttribute, 2, GL_FLOAT, 0, 0, imageVertices);
    glEnableVertexAttribArray(TextureCoordinateAttribute);
    glVertexAttribPointer(TextureCoordinateAttribute, 2, GL_FLOAT, 0, 0, TextureCoordinates);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glReadPixels(0, 0, (GLsizei)(params->pImg->width), (GLsizei)(params->pImg->height),
            GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)(params->pImg->plane[1]));

    mProgram->unbindTexture(GL_TEXTURE0);
    mFbo.unbind(GL_TEXTURE_2D);
    mProgram->unbindTexture(GL_TEXTURE1);
}

