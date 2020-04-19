#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>
#include <math.h>
#include <string>
#include <memory>

enum imageType {RAW, BITMAP, YUV};
enum colorMap {CCD, GRAY, RGB, HSV};

struct image
{
    image(const char * filename, int h, int w);
    image(unsigned char *p, int h, int w);
    image(const std::string& filename);
    image(int h, int w, int channs);
    image(const image& other);
    //image(image&& other) = delete;
    image& operator=(const image& other);
    //image& operator=(image&& other) = delete;

    std::vector<std::vector<std::vector<double>>> pixels; //data
    const int RGB = 3;
    imageType type = BITMAP;
    colorMap color = colorMap::RGB;
    int channels=1;
    int height;
    int width;
    int padd=0;
    int MAX_DEV=255;
    int MIN_DEV=0;

    //reflective Padding
    void padding(int padd_size);  
    //Unpad
    void unPad();
    //Convert to grayscale in RGB from grayscale in one channel
    void grayScale();
    //make all pixels values 0;
    void black();
    //Clip between max, min and Round to int
    template <typename pix>
    inline pix clipAndRound(pix value){
        pix rValue = round(value);
        if(rValue >=MAX_DEV){
            rValue = MAX_DEV;
        }
        else if(rValue <=MIN_DEV){
            rValue = MIN_DEV;
        }
        return rValue;
    }
};

//Subtract two Images
//NO normalization or clipping occurs by default
std::unique_ptr<image> sub(const image& a, const image& b);