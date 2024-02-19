#include <iostream>
#include <fstream>
#include "Windows.h"
#include <vector>

#include "imageData.h"
#include "RGBtoY.h"
#include "additiveNoise.h"
#include "impulseNoise.h"

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
    additiveNoise(fileHeader, infoHeader, Y, H, W, 10, outputPath + "additiveNoise.bmp");
    additiveNoiseGraph(Y, H, W, 5.0, 20.0, 1.0, outputPath + "additiveNoiseGraph.txt");

    // Impulse noise model
    impulseNoise(fileHeader, infoHeader, Y, H, W, 0.1, 0.1, outputPath + "impulseNoise.bmp");
    //impulseNoiseGraph(Y, H, W, 20, 20, outputPath + "impulseNoiseGraph.txt");

    Y.clear();
}
