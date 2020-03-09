//library for image signal processing

#include "isp.h"
#include <fstream>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>
#include <math.h>
#include <string>


void read(const char * filename, void * data, const int size) {
	FILE *file;
	if (!(file = fopen(filename, "rb"))) {
		std::cout << "Cannot open file: " << filename << std::endl;
		exit(1);
	}
	fread(data, sizeof(unsigned char), size, file);
	fclose(file);
}

//Adds Reflective Padding to an Image
void image::padding(int paddSize){
    std::vector<std::vector<unsigned char>> temp(height+2*paddSize, std::vector<unsigned char> (width+2*paddSize));
    //copy pixels into larger vector
    for(auto i = paddSize; i < height + paddSize; i++){
        for(auto j = paddSize; j < width + paddSize; j++){
            temp[i][j] = pixels[i - paddSize][j - paddSize];
        }
    }
    height = height+2*paddSize;
    width = width+2*paddSize;
    pixels = temp;
    //reflection padding corners
    for(auto i = 0; i < paddSize; i++){
        for(auto j = 0; j < paddSize; j++){
            pixels[i][j] = pixels[2*paddSize-1-j][2*paddSize-1-i];
            pixels[i][width-paddSize+j] = pixels[2*paddSize-1-j][width-paddSize-(paddSize-i)];
            pixels[height-paddSize+i][j] = pixels[height-paddSize-(paddSize-j)][2*paddSize-1-i];
            pixels[height-paddSize+i][width-paddSize+j] = pixels[height-paddSize-(paddSize-j)][width-paddSize-(paddSize-i)];
        }
    }
    //reflection padding sides of image
    //top/bottom
    for(auto i = paddSize; i < width - paddSize; i++){
        for(auto j = 0; j < paddSize; j++){
            pixels[j][i] = pixels[2*paddSize-1-j][i];
            pixels[height-paddSize+j][i] =  pixels[height-paddSize-j-1][i];
        }
    }
    //left/right
    for(auto i = paddSize; i < height - paddSize; i++){
        for(auto j = 0; j < paddSize; j++){
            pixels[i][j] = pixels[i][2*paddSize-1-j];
            pixels[i][width-paddSize+j] =  pixels[i][width-paddSize-j-1];
        }
    }
    padd = paddSize;
    height -= 2*padd;
    width -= 2*padd;
}   

void image::unPad(){
    std::vector<std::vector<unsigned char>> temp(height, std::vector<unsigned char> (width));
    //copy inside pixels over
    for(auto i = 0; i < height; ++i){
        for(auto j = 0; j < width; ++j){
            temp[i][j] = pixels[i+padd][j+padd];
        }
    }
    padd = 0;
    pixels = std::move(temp);
}

//creating a uniform convolution kernel
std::vector<std::vector<float>> uniformKern(const int& size){
    std::cout<<size<< " ";
    float elem = 1.0/static_cast<float>(size*size);
    std::cout<<elem<< " ";
    return std::vector<std::vector<float>>(size, std::vector<float> (size, elem));
}

//Convolution between image and kernel
void conv(image& img, const std::vector<std::vector<float>>& kern){
    int paddSize = kern.size()/2;
    std::cout<<kern[0][0];
    img.padding(paddSize);
    auto temp(img.pixels);
    for(auto i = 0; i < img.height; ++i){
        for(auto j = 0; j < img.width; ++j){
            img.pixels[i+paddSize][j+paddSize] = pixConvolution(temp, i+paddSize, j+paddSize, kern);
        }
    }
    img.unPad();
}

//uniform lowpass filter
void uniformFilter(image& img, const int&size){
    std::cout<<size<< " ";
    conv(img, uniformKern(size));
}

//histogram of an image's pixels (not normalized)
std::vector<double> imageHistogram(const std::vector<std::vector<unsigned char>>& img){
    std::vector<double> histogram(256, 0);
    for(auto& i : img){
        for(auto& j : i){
            ++histogram[j];
        }
    }
    return histogram;
}

//CDF of the histogram of an image (not normalized)
std::vector<double> imageCDF(const std::vector<std::vector<unsigned char>>& img){
    auto histogramCdf(imageHistogram(img));
    int temp = 0;
    for(auto& i : histogramCdf){
        i+=temp;
        temp = i;
    }
    return histogramCdf;
}

//Mean Square Error between two images
float mse(const image& imgX, const image& imgY){
    float meanSquareError = 0;
    float maxError = 0;
    float curError = 0;
    for(auto i = 0; i < imgX.height; ++i){
        for(auto j = 0; j < imgX.width; ++j){
            curError = imgX.pixels[i][j] - imgY.pixels[i][j];
            meanSquareError += pow(curError, 2);
        }
    }
    meanSquareError /= (imgX.height*imgY.width);
}

//PSNR - Peak Signal-to-noise Ratio
float psnr(const image& imgX, const image& imgY){
    float mseXY = mse(imgX, imgY);
    return 10*log10(static_cast<float>(pow(imgX.MAX_DEV, 2))/mseXY);
}

//passing in a one channel image, default is linear
void histogramEqualization(std::vector<std::vector<unsigned char>>& img){
    auto hist = imageCDF(img);
    std::vector<double> histPdf(256);
    double numPixels = img.size()*img[0].size();
    size_t count = 0;
    std::for_each(histPdf.begin(), histPdf.end(), [&numPixels, &hist, &count](double& a){
        a = hist[count]/numPixels;
        a*=255;
        ++count;
    });
    std::cout << histPdf[200] << " hi\n";
    for(auto i = 0; i < img.size(); ++i){
        for(auto j = 0; j < img[0].size(); ++j){
            img[i][j] = static_cast<unsigned char>(histPdf[img[i][j]]);
        }
    }
}

const int BILINEARDEMOSAICINGSIZE = 3;

const float brStrongBILINEARDEMOSAICING[3][3] = {
    {0.25,0, 0.25},
    {0,   0, 0},
    {0.25,0, 0.25}};

const float greenStrongBILINEARDEMOSAICING[3][3] = {
    {0,    0.25,0},
    {0.25, 0,   0.25},
    {0,    0.25,0}};

const float brWeakAndBILINEARDEMOSAICING[3][3] = {
    {0,0.5,0},
    {0,0,0},
    {0,0.5,0}};

const float brWeakOrBILINEARDEMOSAICING[3][3] = {
    {0,0,0},
    {0.5,0,0.5},
    {0,0,0}};

const float IDENTITY[3][3] = {
    {0,0,0},
    {0,1,0},
    {0,0,0}};

void bilinearDemosaicing(image& img){
    img.rgb_pixels = std::vector<std::vector<std::vector<unsigned char>>>(img.height, std::vector<std::vector<unsigned char>>(img.width, std::vector<unsigned char>(img.RGB)));
    int padd = static_cast<int>(BILINEARDEMOSAICINGSIZE/2);
    img.padding(padd);

    for(auto i = 0; i < img.height; i++){
        for(auto j = 0; j < img.width; j++){
            if(i%2 == 0 && j%2 == 0){
                img.rgb_pixels[i][j][1] = img.pixels[i+padd][j+padd];
                img.rgb_pixels[i][j][2] = pixConvolution(img, i+padd, j+padd, brWeakAndBILINEARDEMOSAICING);
                img.rgb_pixels[i][j][0] = pixConvolution(img, i+padd, j+padd, brWeakOrBILINEARDEMOSAICING);
            }
            if(i%2 == 0 && j%2 == 1){
                img.rgb_pixels[i][j][1] = pixConvolution(img, i+padd, j+padd, greenStrongBILINEARDEMOSAICING);
                img.rgb_pixels[i][j][2] = pixConvolution(img, i+padd, j+padd, brStrongBILINEARDEMOSAICING);
                img.rgb_pixels[i][j][0] = img.pixels[i+padd][j+padd];
            }
            if(i%2 == 1 && j%2 == 0){
                img.rgb_pixels[i][j][1] = pixConvolution(img, i+padd, j+padd, greenStrongBILINEARDEMOSAICING);
                img.rgb_pixels[i][j][2] = img.pixels[i+padd][j+padd];
                img.rgb_pixels[i][j][0] = pixConvolution(img, i+padd, j+padd, brStrongBILINEARDEMOSAICING);
            }
            if(i%2 == 1 && j%2 == 1){
                img.rgb_pixels[i][j][1] = img.pixels[i+padd][j+padd];
                img.rgb_pixels[i][j][2] = pixConvolution(img, i+padd, j+padd, brWeakOrBILINEARDEMOSAICING);
                img.rgb_pixels[i][j][0] = pixConvolution(img, i+padd, j+padd, brWeakAndBILINEARDEMOSAICING);
            }
        }
    }

    img.unPad();
}

const int MHCDEMOSAICINGSIZE = 5;

const float  greenAtRedBlueMHC[5][5] = {
    {0, 0, -1, 0, 0},
    {0, 0, +2, 0, 0},
    {-1, +2, +4, +2, -1},
    {0, 0, +2, 0, 0},
    {0, 0, -1, 0, 0}
};

const float redBlueAtGreenOddMHC[5][5] = {
    {0, 0, +1/2, 0, 0},
    {0, -1, 0, -1, 0},
    {-1, +4, +5, +4, -1},
    {0, -1, 0, -1, 0},
    {0, 0, +1/2, 0, 0}
};

const float redBlueAtGreenEvenMHC[5][5] = {
    {0, 0, -1, 0, 0},
    {0, -1, 4, -1, 0},
    {+1/2, 0, +5, 0, +1/2},
    {0, -1, 4, -1, 0},
    {0, 0, -1, 0, 0}
};

const float oppRedBlueMHC[5][5] = {
    {0, 0, -3/2, 0, 0},
    {0, 2, 0, 2, 0},
    {-3/2, 0, +6, 0, -3/2},
    {0, 2, 0, 2, 0},
    {0, 0, -3/2, 0, 0}
};

void demosaicingMHC(image& img){
    img.rgb_pixels = std::vector<std::vector<std::vector<unsigned char>>>(img.height, std::vector<std::vector<unsigned char>>(img.width, std::vector<unsigned char>(img.RGB)));
    int padd = static_cast<int>(MHCDEMOSAICINGSIZE/2);
    img.padding(padd);

    for(auto i = 0; i < img.height; i++){
        for(auto j = 0; j < img.width; j++){
            if(i%2 == 0 && j%2 == 0){
                img.rgb_pixels[i][j][1] = clipAndRound(img.pixels[i+padd][j+padd]);
                img.rgb_pixels[i][j][2] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenEvenMHC));
                img.rgb_pixels[i][j][0] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenOddMHC));
            }
            if(i%2 == 0 && j%2 == 1){
                img.rgb_pixels[i][j][1] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, greenAtRedBlueMHC));
                img.rgb_pixels[i][j][2] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, oppRedBlueMHC));
                img.rgb_pixels[i][j][0] = clipAndRound(img.pixels[i+padd][j+padd]);
            }
            if(i%2 == 1 && j%2 == 0){
                img.rgb_pixels[i][j][1] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, greenAtRedBlueMHC));
                img.rgb_pixels[i][j][2] = clipAndRound(img.pixels[i+padd][j+padd]);
                img.rgb_pixels[i][j][0] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, oppRedBlueMHC));
            }
            if(i%2 == 1 && j%2 == 1){
                img.rgb_pixels[i][j][1] = clipAndRound(img.pixels[i+padd][j+padd]);
                img.rgb_pixels[i][j][2] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenOddMHC));
                img.rgb_pixels[i][j][0] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenEvenMHC));
            }
        }
    }

    img.unPad();
}