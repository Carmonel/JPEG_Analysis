#include <iostream>
#include <fstream>
#include "Windows.h"
#include <vector>

#include "Utils.h"
#include "RGBtoY.h"
#include "additiveNoise.h"
#include "impulseNoise.h"
#include "movingAverage.h"

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
    std::vector<std::vector<unsigned char>> additiveY = additiveNoise(fileHeader, infoHeader, Y, H, W, 10, outputPath + "additiveNoise.bmp");
    // 1st - start value
    // 2nd - end value
    // 3rd - step value
    additiveNoiseGraph(Y, H, W, 5.0, 20.0, 0.1, outputPath + "additiveNoiseGraph.txt");

    // Impulse noise model
    std::vector<std::vector<unsigned char>> impulseY = impulseNoise(fileHeader, infoHeader, Y, H, W, 0.1, 0.1, outputPath + "impulseNoise.bmp");
    // 1st - p_a steps count
    // 2nd - p_b steps count
    //impulseNoiseGraph(Y, H, W, 20, 20, outputPath + "impulseNoiseGraph.txt");

    // Moving average
    // Argument - R = [0; R_max]
    int minMovingAverageAddictive = findMinMovingAveragePSNR(fileHeader, infoHeader, additiveY, H, W, 15);
    movingAverage(fileHeader, infoHeader, additiveY, H, W, minMovingAverageAddictive, outputPath + "minMovingAverageAddictive.bmp");

    impulseY.clear();
    additiveY.clear();
    Y.clear();
}
