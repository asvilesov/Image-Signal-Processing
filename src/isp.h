#pragma once

#include <fstream>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>
#include <math.h>
#include <string>

void read(const char * filename, void * data, const int size);

struct image
{
    image(unsigned char *p, int h, int w)
        : padd(0)
        , height(h)
        , width(w)
        , pixels(std::vector<std::vector<unsigned char>>(h, std::vector<unsigned char> (w)))
        {
        for(auto i = 0; i < h; i++){
            for(auto j = 0; j < w; j++){
                pixels[i][j] = p[width*i +j];
            }
        }
    }
    image(const image& other)
        : padd(other.padd)
        , height(other.height)
        , width(other.width)
        , pixels(other.pixels)
        , rgb_pixels(other.rgb_pixels){}
    image(image&& other) = delete;
    image& operator=(const image&) = delete;
    image& operator=(image&& other) = delete;

    void padding(int padd_size);  
    void unPad();

    std::vector<std::vector<unsigned char>> pixels; //one channel: can be grayscale or CCD sensor input
    std::vector<std::vector<std::vector<unsigned char>>> rgb_pixels; //3 RGB channels
    const int RGB = 3;
    int height;
    int width;
    int padd;
    const int MAX_DEV = 255;
};

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
    int sum = 0; 
    for(auto i = 0; i < rows; i++){
        for(auto j = 0; j < cols; j++){
            sum += img.pixels[x-convSize+i][y-convSize+j]*array[i][j];
        }
    }
    return sum;
}

template <class t>
inline float pixConvolution(const std::vector<std::vector<unsigned char>>& img, int x, int y, const std::vector<std::vector<t>>& kernel){
    int padd = static_cast<int>(kernel.size()/2);
    int convSize = kernel.size();
    double sum = 0;
    for(auto i = 0; i < convSize; i++){
        for(auto j = 0; j < convSize; j++){
            sum += kernel[i][j] * static_cast<double>(img[x-padd+i][y-padd+j]);
        }
    }
    return sum;
}

std::vector<std::vector<float>> uniformKern(const int& size);

void conv(image& img, const std::vector<std::vector<float>>& kern);

void uniformFilter(image& img, const int&size);

std::vector<double> imageHistogram(const std::vector<std::vector<unsigned char>>& img);

std::vector<double> imageCDF(const std::vector<std::vector<unsigned char>>& img);

float mse(const image& imgX, const image& imgY);

float psnr(const image& imgX, const image& imgY);

void histogramEqualization(std::vector<std::vector<unsigned char>>& img);

void bilinearDemosaicing(image& img);

void demosaicingMHC(image& img);

