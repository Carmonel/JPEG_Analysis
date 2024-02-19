#ifndef JPEG_ANALYSIS_ADDITIVENOISE_H
#define JPEG_ANALYSIS_ADDITIVENOISE_H

#include <vector>
#include <string>
#include <random>
#include <ctime>

#include "Utils.h"
#include "PSNR.h"
#include "changeFileToDots.h"

void additiveNoiseGraph(const std::vector<std::vector<unsigned char>>& Y, int H, int W, double start, double end, double step, const std::string& outputPath){
    std::ofstream file(outputPath);
    std::random_device rd;
    std::mt19937 gen(rd());

    double gaussianStandartDeviation = start;
    #pragma omp parallel for
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
        #pragma omp critical
        {
            file << "'" << gaussianStandartDeviation << "='" << (std::isinf(psnr)? 0: psnr) << std::endl;
        };
        newYVec.clear();
        gaussianStandartDeviation += step;
    }

    std::cout << outputPath + " created." << std::endl;
    //changeFileToDots(outputPath);
    file.close();
}

std::vector<std::vector<unsigned char>> additiveNoise(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, const std::vector<std::vector<unsigned char>>& Y, int H, int W, double gaussianStandartDeviation, const std::string& outputPath){
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

    std::cout << outputPath + " created; PSNR = " << PSNR(Y, newYVec, H, W) << std::endl;

    file.close();
    return newYVec;
}

#endif //JPEG_ANALYSIS_ADDITIVENOISE_H
