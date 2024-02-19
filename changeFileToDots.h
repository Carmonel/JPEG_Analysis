#ifndef JPEG_ANALYSIS_CHANGEFILETODOTS_H
#define JPEG_ANALYSIS_CHANGEFILETODOTS_H

#include <string>
#include <fstream>
#include <iostream>

void changeFileToDots(std::string filePath){
    std::fstream file(filePath, std::ios::in | std::ios::out);

    if (!file.is_open()) {
        std::cerr << "changeFileToDots(std::string filePath): Failed to open file." << std::endl;
        return;
    }

    char ch;
    while (file.get(ch)) {
        if (ch == '.') {
            file.seekp(-1, std::ios::cur);
            file.put(',');
        }
    }

    file.close();
}

#endif //JPEG_ANALYSIS_CHANGEFILETODOTS_H
