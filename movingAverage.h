#ifndef JPEG_ANALYSIS_MOVINGAVERAGE_H
#define JPEG_ANALYSIS_MOVINGAVERAGE_H

#include <vector>
#include <string>
#include <Windows.h>
#include <fstream>

#include "Utils.h"

int findMinMovingAveragePSNR(std::vector<std::vector<unsigned char>>& Y, int H, int W, int R_max, int sigma){
    std::vector<std::vector<unsigned char>> newYVec(H, std::vector<unsigned char>(W, 0));

    double minPSNR = -4000;
    int R_save;
    #pragma omp parallel for
    for (int R = 0; R < R_max; R++){
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

                newYint /= (2 * R + 1) * (2 * R + 1);
                unsigned char newY = clipping(newYint);
                newYVec[i][j] = newY;
            }
        }
        double psnr = PSNR(Y, newYVec, H, W);
        #pragma omp critical
        {
            if (psnr > minPSNR){
                minPSNR = psnr;
                R_save = R;
            }
        }
    }

    std::cout << "Minimum PSNR for MovingAverage for sigma(" << sigma << ") = " << minPSNR << "; R = " << R_save << std::endl;

    newYVec.clear();
    return R_save;
}

void movingAverage(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, std::vector<std::vector<unsigned char>>& Y, int H, int W, int R, const std::string& outputPath){
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

            newYint /= (2 * R + 1) * (2 * R + 1);
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

#endif //JPEG_ANALYSIS_MOVINGAVERAGE_H
