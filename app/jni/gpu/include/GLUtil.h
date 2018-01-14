//
// Created by lenovo on 2018/1/14.
//

#ifndef _Included_GLUTIL_H
#define _Included_GLUTIL_H

#include <GLES2/gl2.h>
#include "jnilogger.h"

class GLUtil{
    public:
        static void checkGlError(const char* op = NULL) {
            for (GLint error = glGetError(); error != GL_NO_ERROR; error = glGetError()){
                if(op){
                    LOGE("err(%d) after %s \n", error, op);
                }else{
                    LOGE("err(%d)\n", error);
                }
            }
        }
};

#endif //INTERVIEWIMAGEPROC_GLUTIL_H
