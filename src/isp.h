#pragma once

#include "image.h"
#include <fstream>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>
#include <math.h>
#include <string>


template <class pix>
inline pix clipAndRound(pix value){
    pix rValue = round(value);
    if(rValue >=255){
        rValue = 255;
    }
    else if(rValue <=0){
        rValue = 0;
    }
    return rValue;
}

template <size_t rows, size_t cols>
inline float pixConvolution(image& img, int x, int y, const float (&array)[rows][cols]){
    int convSize = int(rows)/2;
    float sum = 0; 
    for(auto i = 0; i < rows; i++){
        for(auto j = 0; j < cols; j++){
            sum += static_cast<float>(img.pixels[0][x-convSize+i][y-convSize+j])*array[i][j];
        }
    }
    return sum;
}

template <class t>
inline float pixConvolution(const std::vector<std::vector<std::vector<int>>>& img, int x, int y, const std::vector<std::vector<t>>& kernel){
    int padd = static_cast<int>(kernel.size()/2);
    int convSize = kernel.size();
    double sum = 0;
    for(auto i = 0; i < convSize; i++){
        for(auto j = 0; j < convSize; j++){
            sum += kernel[i][j] * static_cast<double>(img[0][x-padd+i][y-padd+j]);
        }
    }
    return sum;
}

std::vector<std::vector<float>> uniformKern(const int& size);

void conv(image& img, const std::vector<std::vector<float>>& kern);

void uniformFilter(image& img, const int&size);

std::vector<double> imageHistogram(const std::vector<std::vector<std::vector<int>>>& img);

std::vector<double> imageCDF(const std::vector<std::vector<std::vector<int>>>& img);

float mse(const image& imgX, const image& imgY);

float psnr(const image& imgX, const image& imgY);

void histogramEqualization(std::vector<std::vector<std::vector<int>>>& img);

void bilinearDemosaicing(image& img);

void demosaicingMHC(image& img);

