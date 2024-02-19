#ifndef JPEG_ANALYSIS_ADDITIVENOISE_H
#define JPEG_ANALYSIS_ADDITIVENOISE_H

#include <vector>
#include <string>
#include "imageData.h"
#include <random>
#include <ctime>

void additiveNoise(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, const std::vector<std::vector<unsigned char>>& Y, int H, int W, double gaussianStandartDeviation, const std::string& outputPath){
    std::ofstream file(outputPath, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            std::normal_distribution<> noise(0.0, gaussianStandartDeviation);
            unsigned char newY = clipping(Y[i][j] + noise(gen));
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
        }
    }

    file.close();
}

#endif //JPEG_ANALYSIS_ADDITIVENOISE_H
