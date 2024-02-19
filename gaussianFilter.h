#ifndef JPEG_ANALYSIS_GAUSSIANFILTER_H
#define JPEG_ANALYSIS_GAUSSIANFILTER_H

#include <vector>
#include <string>
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <cmath>

#include "Utils.h"
#include "PSNR.h"

double w(int k, int m, int b){
    return exp((-(k * k + m * m))/(2 * b * b));
}

double Z(int R, int b){
    double result = 0;
    for (int k = -R; k <= R; k++){
        for (int m = -R; m <= R; m++){
            result += w(k, m, b);
        }
    }
    return result;
}

void gaussianFilterGraph(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, std::vector<std::vector<unsigned char>>& Y, int H, int W, int R, int b_max, const std::string& outputPath){
    std::ofstream file(outputPath);

    #pragma omp parallel for
    for (int b = -b_max; b < b_max; b++){
        if (b == 0) continue;
        std::vector<std::vector<unsigned char>> newYVec(H, std::vector<unsigned char>(W, 0));
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                double newYint = 0;
                for (int m = -R; m < R; m++){
                    for (int n = -R; n < R; n++){
                        if ((i + m >= H) || (i + m < 0)) continue;
                        if ((j + n >= W) || (j + n < 0)) continue;
                        newYint += Y[i + m][j + n];
                    }
                }
                newYint /= Z(R, b);
                unsigned char newY = clipping(newYint);
                newYVec[i][j] = newY;
            }
        }
        double psnr = PSNR(Y, newYVec, H, W);
        #pragma omp critical
        {
            file << b << "=" << (std::isinf(psnr)? 0: psnr) << std::endl;
        }
        newYVec.clear();
    }

    std::cout << outputPath + " created." << std::endl;

    file.close();
}

int findMinGaussianFilter(std::vector<std::vector<unsigned char>>& Y, int H, int W, int R, int b_max){
    std::vector<std::vector<unsigned char>> newYVec(H, std::vector<unsigned char>(W, 0));

    double minPSNR = 11110;
    int b_save;
    #pragma omp parallel for
    for (int b = 1; b < b_max; b++){
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                double newYint = 0;
                for (int m = -R; m < R; m++){
                    for (int n = -R; n < R; n++){
                        if ((i + m >= H) || (i + m < 0)) continue;
                        if ((j + n >= W) || (j + n < 0)) continue;
                        newYint += Y[i + m][j + n];
                    }
                }

                newYint /= Z(R, b);
                unsigned char newY = clipping(newYint);
                newYVec[i][j] = newY;
            }
        }
        double psnr = PSNR(Y, newYVec, H, W);
        #pragma omp critical
        {
            if (abs(psnr) < abs(minPSNR)){
                minPSNR = psnr;
                b_save = R;
            }
        }
    }

    std::cout << "Minimum PSNR for GaussianFilter = " << minPSNR << "; b = " << b_save << std::endl;

    newYVec.clear();
    return b_save;
}

void gaussianFilter(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, std::vector<std::vector<unsigned char>>& Y, int H, int W, int R, int b, const std::string& outputPath){
    std::ofstream file(outputPath, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    std::vector<std::vector<unsigned char>> newYVec(H, std::vector<unsigned char>(W, 0));

    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            double newYint = 0;
            for (int m = -R; m < R; m++){
                for (int n = -R; n < R; n++){
                    if ((i + m >= H) || (i + m < 0)) continue;
                    if ((j + n >= W) || (j + n < 0)) continue;
                    newYint += Y[i + m][j + n];
                }
            }

            newYint /= Z(R, b);
            unsigned char newY = clipping(newYint);
            newYVec[i][j] = newY;
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
            file.write(reinterpret_cast<char*>(&newY), sizeof(unsigned char));
        }
    }

    std::cout << outputPath + " created; PSNR = " << PSNR(Y, newYVec, H, W) << std::endl;

    newYVec.clear();
    file.close();
}

#endif //JPEG_ANALYSIS_GAUSSIANFILTER_H
