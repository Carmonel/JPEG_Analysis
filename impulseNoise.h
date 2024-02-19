#ifndef JPEG_ANALYSIS_IMPULSENOISE_H
#define JPEG_ANALYSIS_IMPULSENOISE_H

#include "Windows.h"
#include <vector>
#include <string>
#include <fstream>
#include <random>

void impulseNoise(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, std::vector<std::vector<unsigned char>>& Y, int H, int W, double p_a, double p_b, const std::string& outputPath){
    std::ofstream file(outputPath, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            double rand = dis(gen);
            if (rand < p_a){
                unsigned char null = 0;
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                continue;
            }
            if (rand < p_a + p_b){
                unsigned char null = 255;
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&null), sizeof(unsigned char));
                continue;
            }
            file.write(reinterpret_cast<char*>(&Y[i][j]), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&Y[i][j]), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&Y[i][j]), sizeof(unsigned char));
        }
    }

    file.close();
}

#endif //JPEG_ANALYSIS_IMPULSENOISE_H
