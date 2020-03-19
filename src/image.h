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

enum imageType {RAW, BITMAP, YUV};
enum colorMap {CCD, GRAY, RGB, HSV};

struct image
{
    image(const char * filename, int h, int w);
    image(unsigned char *p, int h, int w);
    image(const std::string& filename);
    image(const image& other);
    image(image&& other) = delete;
    image& operator=(const image&) = delete;
    image& operator=(image&& other) = delete;

    std::vector<std::vector<std::vector<int>>> pixels; //data
    const int RGB = 3;
    imageType type;
    colorMap color;
    int channels;
    int height;
    int width;
    int padd;
    int MAX_DEV;
    int MIN_DEV;

    void padding(int padd_size);  
    void unPad();
    void grayScale();
    
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