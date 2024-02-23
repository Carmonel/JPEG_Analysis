#ifndef JPEG_ANALYSIS_PARALLEL_H
#define JPEG_ANALYSIS_PARALLEL_H

#include <vector>
#include <string>
#include <thread>
#include "Windows.h"
#include "movingAverage.h"

void findMinMovingAveragePSNR(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, int H, int W,
                              std::vector<std::vector<unsigned char>>& additiveY80, std::vector<std::vector<unsigned char>>& additiveY50,
                              std::vector<std::vector<unsigned char>>& additiveY30, std::vector<std::vector<unsigned char>>& additiveY10,
                              std::vector<std::vector<unsigned char>>& additiveY1, const std::string& outputPath){
    std::vector<std::thread> threads;









}

#endif //JPEG_ANALYSIS_PARALLEL_H
