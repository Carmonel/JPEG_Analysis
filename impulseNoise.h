#ifndef JPEG_ANALYSIS_IMPULSENOISE_H
#define JPEG_ANALYSIS_IMPULSENOISE_H

#include "Windows.h"
#include <vector>
#include <string>
#include <fstream>
#include <random>

#include "changeFileToDots.h"

void impulseNoiseGraph(std::vector<std::vector<unsigned char>>& Y, int H, int W, int i, int j, const std::string& outputPath){
    std::ofstream file(outputPath);
    double stepM = 1.0 / abs(i);
    double stepN = 1.0 / abs(j);
    double m = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    #pragma omp parallel for
    for (int m = 0; m <= i; m++){
        double n = 0;
        while (n <= 1){
            double p_a = m * stepM;
            double p_b = n;
            std::vector<std::vector<unsigned char>> newYVec(H, std::vector<unsigned char>(W, 0));

            for (int i = 0; i < H; i++){
                for (int j = 0; j < W; j++){
                    double rand = dis(gen);
                    if (rand < p_a){
                        unsigned char null = 0;
                        newYVec[i][j] = null;
                        continue;
                    }
                    if (rand < p_a + p_b){
                        unsigned char null = 255;
                        newYVec[i][j] = null;
                        continue;
                    }
                    newYVec[i][j] = Y[i][j];
                }
            }
            double psnr = PSNR(Y, newYVec, H, W);
            #pragma omp critical
            {
                file << m * stepM << "=" << n << "=" << (std::isinf(psnr)? 0: psnr) << std::endl;
            }
            newYVec.clear();
            n += stepN;
        }
    }
    std::cout << outputPath + " created." << std::endl;

    file.close();

    changeFileToDots(outputPath);
}

std::vector<std::vector<unsigned char>> impulseNoise(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, std::vector<std::vector<unsigned char>>& Y, int H, int W, double p_a, double p_b, const std::string& outputPath){
    std::ofstream file(outputPath, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    std::vector<std::vector<unsigned char>> newYVec(H, std::vector<unsigned char>(W, 0));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            double rand = dis(gen);
            if (rand < p_a){
                unsigned char null = 0;
                newYVec[i][j] = null;
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                continue;
            }
            if (rand < p_a + p_b){
                unsigned char null = 255;
                newYVec[i][j] = null;
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                continue;
            }
            newYVec[i][j] = Y[i][j];
            file.write(reinterpret_cast<char*>(&Y[i][j]), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&Y[i][j]), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&Y[i][j]), sizeof(unsigned char));
        }
    }

    std::cout << outputPath + " created; PSNR = " << PSNR(Y, newYVec, H, W) << std::endl;

    file.close();
    return newYVec;
}

#endif //JPEG_ANALYSIS_IMPULSENOISE_H
