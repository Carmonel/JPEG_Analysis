#ifndef JPEG_ANALYSIS_RGBTOY_H
#define JPEG_ANALYSIS_RGBTOY_H

#include <vector>

#include "imageData.h"

std::vector<std::vector<unsigned char>> RGBtoY(const std::vector<std::vector<RGBPixel>>& pixels, int H, int W){
    std::vector<std::vector<unsigned char>> result(H, std::vector<unsigned char>(W, 0));

    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            result[i][j] = clipping(0.229*pixels[i][j].R + 0.587*pixels[i][j].G + 0.114*pixels[i][j].B);
        }
    }

    return result;
}


#endif //JPEG_ANALYSIS_RGBTOY_H
