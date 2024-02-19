#include <iostream>
#include <fstream>
#include "Windows.h"
#include <vector>

#include "Utils.h"
#include "RGBtoY.h"
#include "additiveNoise.h"
#include "impulseNoise.h"
#include "movingAverage.h"
#include "gaussianFilter.h"
#include "medianFilter.h"
#include "laplaceOperator.h"

int main(int argc, char* argv[]) {
    if (argc != 3){
        std::cerr << "Not enough arguments." << std::endl;
        std::cerr << "Using: -inputFile -outputDirectory" << std::endl;
        std::cerr << "Example: run.exe E:\\image\\image.bmp E:\\image" << std::endl;
        exit(1);
    }
    std::string outputPath(argv[2]);

    // File reading
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    std::ifstream file(argv[1], std::ios::in | std::ios::binary);
    if (!file.is_open()){
        std::cerr << "File not opened.";
        exit(-1);
    }
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    int H = infoHeader.biHeight;
    int W = infoHeader.biWidth;
    std::vector<RGBPixel> pixelsVec(H * W, 0);
    file.read(reinterpret_cast<char*>(pixelsVec.data()), infoHeader.biSizeImage);
    file.close();
    std::cout << "File read." << std::endl << std::endl;
    std::vector<std::vector<RGBPixel>> pixels(H, std::vector<RGBPixel>(W, 0));
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            pixels[i][j] = pixelsVec[i * W + j];
        }
    }
    pixelsVec.clear();

    // RGB to Y
    std::vector<std::vector<unsigned char>> Y = RGBtoY(pixels, H, W);
    pixels.clear();

    // Additive noise model with Gaussian random variable with distribution N(0, σ^2)
    //std::vector<std::vector<unsigned char>> additiveY = additiveNoise(fileHeader, infoHeader, Y, H, W, 10, outputPath + "additiveNoise.bmp");
    // 1st - start value
    // 2nd - end value
    // 3rd - step value
    //additiveNoiseGraph(Y, H, W, 5.0, 20.0, 0.1, outputPath + "additiveNoiseGraph.txt");

    // Impulse noise model
    //std::vector<std::vector<unsigned char>> impulseY = impulseNoise(fileHeader, infoHeader, Y, H, W, 0.1, 0.1, outputPath + "impulseNoise.bmp");
    //impulseNoise(fileHeader, infoHeader, Y, H, W, 0.025, 0.025, outputPath + "impulseNoise_5.bmp");
    //impulseNoise(fileHeader, infoHeader, Y, H, W, 0.05, 0.05, outputPath + "impulseNoise_10.bmp");
    //impulseNoise(fileHeader, infoHeader, Y, H, W, 0.175, 0.175, outputPath + "impulseNoise_25.bmp");
    //impulseNoise(fileHeader, infoHeader, Y, H, W, 0.25, 0.25, outputPath + "impulseNoise_50.bmp");
    // 1st - p_a steps count
    // 2nd - p_b steps count
    //impulseNoiseGraph(Y, H, W, 20, 20, outputPath + "impulseNoiseGraph.txt");

    // Moving average
    // Argument - R = [0; R_max]
    //int minMovingAverageAddictive = findMinMovingAveragePSNR(additiveY, H, W, 15);
    //movingAverage(fileHeader, infoHeader, additiveY, H, W, minMovingAverageAddictive, outputPath + "minMovingAverageAddictive.bmp");

    // Gaussian Filter
    // Argument - b = [1; b_max]
    //int minGaussianFilterAddictive = findMinGaussianFilter(additiveY, H, W, 2, 10);
    //gaussianFilter(fileHeader, infoHeader, additiveY, H, W, 2, minGaussianFilterAddictive, outputPath + "minGaussianFilterAddictive.bmp");
    // Argument - b = [-b_max; b_max]
    //gaussianFilterGraph(fileHeader, infoHeader, additiveY, H, W, 2, 10, outputPath + "gaussianFilterGraph_2.txt");
    //gaussianFilterGraph(fileHeader, infoHeader, additiveY, H, W, 3, 10, outputPath + "gaussianFilterGraph_3.txt");
    //gaussianFilterGraph(fileHeader, infoHeader, additiveY, H, W, 5, 10, outputPath + "gaussianFilterGraph_5.txt");

    // Median Filter
    //int minMedianFilterAddictive = findMinGaussianFilter(additiveY, H, W, 2, 10);
    //medianFilter(fileHeader, infoHeader, additiveY, H, W, minMedianFilterAddictive, outputPath + "minGaussianFilterAddictive.bmp");

    // Laplace operator
    laplaceOperator(fileHeader, infoHeader, Y, H, W, outputPath + "laplaceOperator.bmp");

    //impulseY.clear();
    //additiveY.clear();
    Y.clear();
}
