#include "image.h"

image::image(const char * filename, int h, int w)
    : padd(0)
    , height(h)
    , width(w)
    , pixels(std::vector<std::vector<std::vector<int>>>(1, std::vector<std::vector<int>>(h, std::vector<int> (w)))) 
    , channels(1)
    , MAX_DEV(255)
    , MIN_DEV(0)
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
    , MAX_DEV(255)
    , MIN_DEV(0)
    {
        for(auto i = 0; i < h; i++){
            for(auto j = 0; j < w; j++){
                pixels[0][i][j] = p[width*i +j];
            }
        }
}

image::image(const std::string& filename)
    : padd(0)
    , MAX_DEV(255)
    , MIN_DEV(0)
    {
        cv::Mat img;
        img = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
        height = img.rows;
        width = img.cols;
        channels = img.channels();
        pixels = std::vector<std::vector<std::vector<int>>>(3, std::vector<std::vector<int>>(height, std::vector<int> (width)));
        unsigned char* ptr = nullptr;
        for(auto i = 0; i < height; i++){
            for(auto j = 0; j<width; j++){
                for(auto k = 0; k < img.channels(); ++k){
                    ptr = img.ptr(i, j);
                    pixels[k][i][j] = ptr[k];
                }
            }
        }
    }

image::image(const image& other)
    : padd(other.padd)
    , height(other.height)
    , width(other.width)
    , pixels(other.pixels)
    , channels(other.channels)
    , MAX_DEV(other.MAX_DEV)
    , MIN_DEV(other.MIN_DEV)
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
    pixels = std::move(temp);
}

void image::grayScale(){
    if(channels == 3){
        double tempPixel;
        std::vector<std::vector<std::vector<int>>> temp(1, std::vector<std::vector<int>>(height, std::vector<int> (width)));
        for(auto i = 0; i < height; ++i){
            for(auto j = 0; j < width; ++j){
                tempPixel = 0.2989 * pixels[0][i][j] + 0.5870 * pixels[1][i][j] + 0.1140 * pixels[2][i][j];
                temp[0][i][j] = tempPixel;
            }
        }
        channels = 1;
    }

}