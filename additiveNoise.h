#ifndef JPEG_ANALYSIS_ADDITIVENOISE_H
#define JPEG_ANALYSIS_ADDITIVENOISE_H

#include <vector>
#include <string>
#include <random>
#include <ctime>

#include "imageData.h"
#include "PSNR.h"

void additiveNoiseGraph(const std::vector<std::vector<unsigned char>>& Y, int H, int W, double start, double end, double step, const std::string& outputPath){
    std::ofstream file(outputPath);
    std::random_device rd;
    std::mt19937 gen(rd());

    double gaussianStandartDeviation = start;
    while (gaussianStandartDeviation <= end){
        std::vector<std::vector<unsigned char>> newYVec(H, std::vector<unsigned char>(W, 0));
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                std::normal_distribution<> noise(0.0, gaussianStandartDeviation);
                unsigned char newY = clipping(Y[i][j] + noise(gen));
                newYVec[i][j] = newY;
            }
        }
        double psnr = PSNR(Y, newYVec, H, W);
        file << gaussianStandartDeviation << "=" << (std::isinf(psnr)? 0: psnr) << std::endl;
        newYVec.clear();
        gaussianStandartDeviation += step;
    }

    std::cout << "additiveNoiseGraph.txt created." << std::endl;

    file.close();
}

void additiveNoise(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, const std::vector<std::vector<unsigned char>>& Y, int H, int W, double gaussianStandartDeviation, const std::string& outputPath){
    std::ofstream file(outputPath, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    std::vector<std::vector<unsigned char>> newYVec(H, std::vector<unsigned char>(W, 0));

    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            std::normal_distribution<> noise(0.0, gaussianStandartDeviation);
            unsigned char newY = clipping(Y[i][j] + noise(gen));
            newYVec[i][j] = newY;
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
        }
    }

    std::cout << "additiveNoise.bmp created." << std::endl;
    std::cout << "PSNR = " << PSNR(Y, newYVec, H, W) << std::endl;

    newYVec.clear();
    file.close();
}

#endif //JPEG_ANALYSIS_ADDITIVENOISE_H
