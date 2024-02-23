#ifndef JPEG_ANALYSIS_LAPLACEOPERATOR_H
#define JPEG_ANALYSIS_LAPLACEOPERATOR_H

#include "Windows.h"
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <iostream>

std::vector<std::vector<unsigned char>> laplaceOperator(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, std::vector<std::vector<unsigned char>>& Y, int H, int W, const std::string& outputPath){
    std::ofstream file(outputPath, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    std::vector<std::vector<unsigned char>> newYVec(H, std::vector<unsigned char>(W, 0));

    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            if ((i == H - 1) || (i == 0) || (j == W - 1) || (j == 0)){
                newYVec[i][j] = Y[i][j];
                file.write(reinterpret_cast<char*>(&Y[i][j]), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&Y[i][j]), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&Y[i][j]), sizeof(unsigned char));
                continue;
            }
            unsigned char newY = clipping(Y[i][j] + ((Y[i][j + 1] + Y[i][j - 1] + Y[i + 1][j] + Y[i - 1][j]) - 4 * Y[i][j]));
            newYVec[i][j] = newY;
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
        }
    }

    std::cout << outputPath + " created; PSNR = " << PSNR(Y, newYVec, H, W) << std::endl;

    file.close();
    return newYVec;
}

#endif //JPEG_ANALYSIS_LAPLACEOPERATOR_H
