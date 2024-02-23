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
#include "parallel.h"

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

    std::vector<std::thread> threads;

    ///
    /// Additive noise model with Gaussian random variable with distribution N(0, σ^2) (1, 3)
    ///
    auto computeAdditiveNoise = [](BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, const std::vector<std::vector<unsigned char>>& Y,
                                   int H, int W, int noiseLevel, const std::string& outputPath) {
        return additiveNoise(fileHeader, infoHeader, Y, H, W, noiseLevel, outputPath);
    };
    std::vector<std::vector<unsigned char>> additiveY80, additiveY50, additiveY30, additiveY10, additiveY1;
    threads.emplace_back([&]() { additiveY80 = computeAdditiveNoise(fileHeader, infoHeader, Y, H, W, 80, outputPath + "AdditiveNoise\\additiveNoise80.bmp"); });
    threads.emplace_back([&]() { additiveY50 = computeAdditiveNoise(fileHeader, infoHeader, Y, H, W, 50, outputPath + "AdditiveNoise\\additiveNoise50.bmp"); });
    threads.emplace_back([&]() { additiveY30 = computeAdditiveNoise(fileHeader, infoHeader, Y, H, W, 30, outputPath + "AdditiveNoise\\additiveNoise30.bmp"); });
    threads.emplace_back([&]() { additiveY10 = computeAdditiveNoise(fileHeader, infoHeader, Y, H, W, 10, outputPath + "AdditiveNoise\\additiveNoise10.bmp"); });
    threads.emplace_back([&]() { additiveY1 = computeAdditiveNoise(fileHeader, infoHeader, Y, H, W, 1, outputPath + "AdditiveNoise\\additiveNoise1.bmp"); });
    for (auto& t : threads) {
        t.join();
    }
    // 1st - start value
    // 2nd - end value
    // 3rd - step value
    //additiveNoiseGraph(Y, H, W, 5.0, 20.0, 1, outputPath + "AdditiveNoise\\additiveNoiseGraph.txt");
    threads.clear();

    ///
    /// Impulse noise model (2, 3)
    ///
    impulseNoise(fileHeader, infoHeader, Y, H, W, 0.025, 0.025, outputPath + "impulseNoise_5.bmp");
    impulseNoise(fileHeader, infoHeader, Y, H, W, 0.05, 0.05, outputPath + "impulseNoise_10.bmp");
    impulseNoise(fileHeader, infoHeader, Y, H, W, 0.125, 0.125, outputPath + "impulseNoise_25.bmp");
    impulseNoise(fileHeader, infoHeader, Y, H, W, 0.25, 0.25, outputPath + "impulseNoise_50.bmp");


    ///
    /// Moving average (4.1 - 4.2)
    ///
    auto calculateMinMovingAverage = [](std::vector<std::vector<unsigned char>>& additiveY, int H, int W, int value) {
        return findMinMovingAveragePSNR(additiveY, H, W, 15, value);
    };
    int minMovingAverageAddictive80, minMovingAverageAddictive50, minMovingAverageAddictive30, minMovingAverageAddictive10, minMovingAverageAddictive1;
    threads.emplace_back([&](){ minMovingAverageAddictive80 = calculateMinMovingAverage(additiveY80, H, W, 80); });
    threads.emplace_back([&](){ minMovingAverageAddictive50 = calculateMinMovingAverage(additiveY50, H, W, 50); });
    threads.emplace_back([&](){ minMovingAverageAddictive30 = calculateMinMovingAverage(additiveY30, H, W, 30); });
    threads.emplace_back([&](){ minMovingAverageAddictive10 = calculateMinMovingAverage(additiveY10, H, W, 10); });
    threads.emplace_back([&](){ minMovingAverageAddictive1 = calculateMinMovingAverage(additiveY1, H, W, 1); });
    for (auto& t : threads) {
        t.join();
    }
    threads.clear();
    movingAverage(fileHeader, infoHeader, additiveY50, H, W, minMovingAverageAddictive50, outputPath + "MovingAverage\\minMovingAverageAddictive50.bmp");
    movingAverage(fileHeader, infoHeader, additiveY30, H, W, minMovingAverageAddictive30, outputPath + "MovingAverage\\minMovingAverageAddictive30.bmp");
    movingAverage(fileHeader, infoHeader, additiveY10, H, W, minMovingAverageAddictive10, outputPath + "MovingAverage\\minMovingAverageAddictive10.bmp");
    movingAverage(fileHeader, infoHeader, additiveY1, H, W, minMovingAverageAddictive1, outputPath + "MovingAverage\\minMovingAverageAddictive1.bmp");
    movingAverage(fileHeader, infoHeader, additiveY80, H, W, minMovingAverageAddictive80, outputPath + "MovingAverage\\minMovingAverageAddictive80.bmp");

    ///
    /// Gaussian Filter
    ///
    // Argument - b = [1; b_max]
    gaussianFilterGraph(additiveY80, H, W, 1, outputPath + "GaussianFilter\\GF_80_1.txt");
    gaussianFilterGraph(additiveY50, H, W, 1, outputPath + "GaussianFilter\\GF_50_1.txt");
    gaussianFilterGraph(additiveY30, H, W, 1, outputPath + "GaussianFilter\\GF_30_1.txt");
    gaussianFilterGraph(additiveY10, H, W, 1, outputPath + "GaussianFilter\\GF_10_1.txt");
    gaussianFilterGraph(additiveY1, H, W, 1, outputPath + "GaussianFilter\\GF_1_1.txt");

    gaussianFilterGraph(additiveY80, H, W, 3, outputPath + "GaussianFilter\\GF_80_3.txt");
    gaussianFilterGraph(additiveY50, H, W, 3, outputPath + "GaussianFilter\\GF_50_3.txt");
    gaussianFilterGraph(additiveY30, H, W, 3, outputPath + "GaussianFilter\\GF_30_3.txt");
    gaussianFilterGraph(additiveY10, H, W, 3, outputPath + "GaussianFilter\\GF_10_3.txt");
    gaussianFilterGraph(additiveY1, H, W, 3, outputPath + "GaussianFilter\\GF_1_3.txt");

    gaussianFilterGraph(additiveY80, H, W, 5, outputPath + "GaussianFilter\\GF_80_5.txt");
    gaussianFilterGraph(additiveY50, H, W, 5, outputPath + "GaussianFilter\\GF_50_5.txt");
    gaussianFilterGraph(additiveY30, H, W, 5, outputPath + "GaussianFilter\\GF_30_5.txt");
    gaussianFilterGraph(additiveY10, H, W, 5, outputPath + "GaussianFilter\\GF_10_5.txt");
    gaussianFilterGraph(additiveY1, H, W, 5, outputPath + "GaussianFilter\\GF_1_5.txt");

    ///
    /// Median Filter
    ///
    //int minMedianFilterAddictive = findM(additiveY80, H, W, 2, 10);
    //medianFilter(fileHeader, infoHeader, additiveY80, H, W, minMedianFilterAddictive, outputPath + "MedianFilter\\minMedianFilterAddictive.bmp");

    //impulseY.clear();
    //additiveY.clear();
    Y.clear();
}
