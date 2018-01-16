//
// Created on 2018/1/16.
//

#ifndef INTERVIEWIMAGEPROC_MAKEUPPARAMS_H
#define INTERVIEWIMAGEPROC_MAKEUPPARAMS_H
#include "Image.h"

enum MakeupType{
    SKINWHITEN,
    FACECLEAN
};

class MakeupParams{
    public :
        MakeupParams(){pImg = 0;}
        Image *pImg;
        MakeupType type;
};
#endif //INTERVIEWIMAGEPROC_MAKEUPPARAMS_H
