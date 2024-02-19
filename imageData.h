#ifndef JPEG_ANALYSIS_IMAGEDATA_H
#define JPEG_ANALYSIS_IMAGEDATA_H

#include <fstream>

#include "Windows.h"

template<class T>
unsigned char clipping(T a){
    if (a > 255) return 255;
    if (a < 0) return 0;
    return (unsigned char) a;
}


struct RGBPixel{
    unsigned char R;
    unsigned char G;
    unsigned char B;
    RGBPixel(int i){
        this->R = i;
        this->G = i;
        this->B = i;
    }
};

#endif //JPEG_ANALYSIS_IMAGEDATA_H
