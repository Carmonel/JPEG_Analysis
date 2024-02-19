#ifndef JPEG_ANALYSIS_PSNR_H
#define JPEG_ANALYSIS_PSNR_H

#include <vector>
#include <cmath>

double PSNR(const std::vector<std::vector<unsigned char>>& a, const std::vector<std::vector<unsigned char>>& b, int H, int W){
    double PSNR = 0;
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            PSNR += (a[i][j] - b[i][j]) * (a[i][j] - b[i][j]);
        }
    }
    return 10 * log(W * H * (pow(2, 8) - 1) / PSNR);
}

#endif //JPEG_ANALYSIS_PSNR_H
