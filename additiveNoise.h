#ifndef JPEG_ANALYSIS_ADDITIVENOISE_H
#define JPEG_ANALYSIS_ADDITIVENOISE_H

#include <vector>
#include <string>
#include <random>
#include <ctime>

#include "Utils.h"
#include "PSNR.h"

double BoxMuller(double gaussianStandartDeviation){
    // Переменные Бокса-Мюллера
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> uniDist(0.0, 1.0);

    // н.с.в.
    double u1 = uniDist(gen);
    double u2 = uniDist(gen);
    // Бокс-Мюллер
    double z = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
    // Комбинация для управления
    double noise = gaussianStandartDeviation * z;

    return noise;
}

void additiveNoiseGraph(const std::vector<std::vector<unsigned char>>& Y, int H, int W, double start, double end, double step, const std::string& outputPath){
    std::ofstream file(outputPath);

    double gaussianStandartDeviation = start;
    #pragma omp parallel for
    while (gaussianStandartDeviation <= end){
        std::vector<std::vector<unsigned char>> newYVec(H, std::vector<unsigned char>(W, 0));
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                unsigned char newY = clipping(Y[i][j] + BoxMuller(gaussianStandartDeviation));
                newYVec[i][j] = newY;
            }
        }
        double psnr = PSNR(Y, newYVec, H, W);
        #pragma omp critical
        {
            file << gaussianStandartDeviation << "=" << (std::isinf(psnr)? 0: psnr) << std::endl;
        };
        newYVec.clear();
        gaussianStandartDeviation += step;
    }

    std::cout << outputPath + " created." << std::endl;
    file.close();
}

std::vector<std::vector<unsigned char>> additiveNoise(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, const std::vector<std::vector<unsigned char>>& Y, int H, int W, double gaussianStandartDeviation, const std::string& outputPath){
    std::ofstream file(outputPath, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    std::vector<std::vector<unsigned char>> newYVec(H, std::vector<unsigned char>(W, 0));

    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            unsigned char newY = clipping(Y[i][j] + BoxMuller(gaussianStandartDeviation));
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
