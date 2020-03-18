#pragma once

#include <fstream>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>
#include <math.h>
#include <string>

enum imageType {MHC, BITMAP, YUV};
enum colorMap {GRAY, RGB, HSV};

struct image
{
    image(const char * filename, int h, int w);
    image(unsigned char *p, int h, int w);
    image(const image& other);
    image(image&& other) = delete;
    image& operator=(const image&) = delete;
    image& operator=(image&& other) = delete;

    void padding(int padd_size);  
    void unPad();

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

};