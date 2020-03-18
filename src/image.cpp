#include "image.h"

image::image(const char * filename, int h, int w)
    : padd(0)
    , height(h)
    , width(w)
    , pixels(std::vector<std::vector<std::vector<int>>>(1, std::vector<std::vector<int>>(h, std::vector<int> (w)))) 
    , channels(1)
    {
        FILE *file;
        if (!(file = fopen(filename, "rb"))) {
            std::cout << "Cannot open file: " << filename << std::endl;
            exit(1);
        }
        uint8_t data[height*width];
        int size = height*width;
        fread(data, sizeof(uint8_t), size, file);
        fclose(file);
        auto imgData = reinterpret_cast<uint8_t*>(data);
        for(auto i = 0; i < h; i++){
            for(auto j = 0; j < w; j++){
                pixels[0][i][j] = imgData[width*i +j];
            }
        }
}

image::image(unsigned char *p, int h, int w)
    : padd(0)
    , height(h)
    , width(w)
    , pixels(std::vector<std::vector<std::vector<int>>>(1, std::vector<std::vector<int>>(h, std::vector<int> (w))))
    , channels(1)
    {
        for(auto i = 0; i < h; i++){
            for(auto j = 0; j < w; j++){
                pixels[0][i][j] = p[width*i +j];
            }
        }
}

image::image(const image& other)
    : padd(other.padd)
    , height(other.height)
    , width(other.width)
    , pixels(other.pixels)
    {}

//Adds Reflective Padding to an Image
void image::padding(int paddSize){
    std::vector<std::vector<std::vector<int>>> temp(1, std::vector<std::vector<int>>(height+2*paddSize, std::vector<int> (width+2*paddSize)));
    //copy pixels into larger vector
    for(auto i = paddSize; i < height + paddSize; i++){
        for(auto j = paddSize; j < width + paddSize; j++){
            temp[0][i][j] = pixels[0][i - paddSize][j - paddSize];
        }
    }
    height = height+2*paddSize;
    width = width+2*paddSize;
    pixels = temp;
    //reflection padding corners
    for(auto i = 0; i < paddSize; i++){
        for(auto j = 0; j < paddSize; j++){
            pixels[0][i][j] = pixels[0][2*paddSize-1-j][2*paddSize-1-i];
            pixels[0][i][width-paddSize+j] = pixels[0][2*paddSize-1-j][width-paddSize-(paddSize-i)];
            pixels[0][height-paddSize+i][j] = pixels[0][height-paddSize-(paddSize-j)][2*paddSize-1-i];
            pixels[0][height-paddSize+i][width-paddSize+j] = pixels[0][height-paddSize-(paddSize-j)][width-paddSize-(paddSize-i)];
        }
    }
    //reflection padding sides of image
    //top/bottom
    for(auto i = paddSize; i < width - paddSize; i++){
        for(auto j = 0; j < paddSize; j++){
            pixels[0][j][i] = pixels[0][2*paddSize-1-j][i];
            pixels[0][height-paddSize+j][i] =  pixels[0][height-paddSize-j-1][i];
        }
    }
    //left/right
    for(auto i = paddSize; i < height - paddSize; i++){
        for(auto j = 0; j < paddSize; j++){
            pixels[0][i][j] = pixels[0][i][2*paddSize-1-j];
            pixels[0][i][width-paddSize+j] =  pixels[0][i][width-paddSize-j-1];
        }
    }
    padd = paddSize;
    height -= 2*padd;
    width -= 2*padd;
}   

void image::unPad(){
    std::vector<std::vector<std::vector<int>>> temp(1, std::vector<std::vector<int>>(height, std::vector<int> (width)));
    //copy inside pixels over
    for(auto i = 0; i < height; ++i){
        for(auto j = 0; j < width; ++j){
            temp[0][i][j] = pixels[0][i+padd][j+padd];
        }
    }
    padd = 0;
    // pixels = std::move(temp);
    pixels = temp;
}