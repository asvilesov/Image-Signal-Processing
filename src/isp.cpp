//library for image signal processing

#include "isp.h"
#include <fstream>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>
#include <math.h>
#include <string>

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
    img.padding(paddSize);
    auto temp(img.pixels);
    for(auto i = 0; i < img.height; ++i){
        for(auto j = 0; j < img.width; ++j){
            img.pixels[0][i+paddSize][j+paddSize] = pixConvolution(temp, i+paddSize, j+paddSize, kern);
        }
    }
    img.unPad();
}

//uniform lowpass filter
void uniformFilter(image& img, const int&size){
    conv(img, uniformKern(size));
}

//histogram of an image's pixels (not normalized) (assumes one channel)
std::vector<double> imageHistogram(const std::vector<std::vector<std::vector<int>>>& img){
    std::vector<double> histogram(256, 0);
    for(auto& i : img[0]){
        for(auto& j : i){
            ++histogram[j];
        }
    }
    return histogram;
}

//CDF of the histogram of an image (not normalized)
std::vector<double> imageCDF(const std::vector<std::vector<std::vector<int>>>& img){
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
    for(auto k = 0; k < imgX.channels; ++k){
        for(auto i = 0; i < imgX.height; ++i){
            for(auto j = 0; j < imgX.width; ++j){
                curError = imgX.pixels[k][i][j] - imgY.pixels[k][i][j];
                meanSquareError += pow(curError, 2);
            }
        }
    }
    meanSquareError /= (imgX.channels*imgX.height*imgX.width);
}

//PSNR - Peak Signal-to-noise Ratio
float psnr(const image& imgX, const image& imgY){
    float mseXY = mse(imgX, imgY);
    return 10*log10(static_cast<float>(pow(imgX.MAX_DEV, 2))/mseXY);
}

//passing in a one channel image, default is linear ***assumes one channel
void histogramEqualization(std::vector<std::vector<std::vector<int>>>& img){
    auto hist = imageCDF(img);
    std::vector<double> histPdf(256);
    double numPixels = img[0].size()*img[0][0].size();
    size_t count = 0;
    std::for_each(histPdf.begin(), histPdf.end(), [&numPixels, &hist, &count](double& a){
        a = hist[count]/numPixels;
        a*=255;
        ++count;
    });
    for(auto i = 0; i < img[0].size(); ++i){
        for(auto j = 0; j < img[0][0].size(); ++j){
            img[0][i][j] = static_cast<int>(histPdf[img[0][i][j]]);
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
    std::vector<std::vector<std::vector<int>>> temp(img.RGB, std::vector<std::vector<int>>(img.height, std::vector<int>(img.width)));
    int padd = static_cast<int>(BILINEARDEMOSAICINGSIZE/2);
    img.padding(padd);

    for(auto i = 0; i < img.height; i++){
        for(auto j = 0; j < img.width; j++){
            if(i%2 == 0 && j%2 == 0){
                temp[1][i][j] = img.pixels[0][i+padd][j+padd];
                temp[2][i][j] = clipAndRound(pixConvolution(img, i+padd, j+padd, brWeakAndBILINEARDEMOSAICING));
                temp[0][i][j] = clipAndRound(pixConvolution(img, i+padd, j+padd, brWeakOrBILINEARDEMOSAICING));
            }
            if(i%2 == 0 && j%2 == 1){
                temp[1][i][j] = clipAndRound(pixConvolution(img, i+padd, j+padd, greenStrongBILINEARDEMOSAICING));
                temp[2][i][j] = clipAndRound(pixConvolution(img, i+padd, j+padd, brStrongBILINEARDEMOSAICING));
                temp[0][i][j] = img.pixels[0][i+padd][j+padd];
            }
            if(i%2 == 1 && j%2 == 0){
                temp[1][i][j] = clipAndRound(pixConvolution(img, i+padd, j+padd, greenStrongBILINEARDEMOSAICING));
                temp[2][i][j] = img.pixels[0][i+padd][j+padd];
                temp[0][i][j] = clipAndRound(pixConvolution(img, i+padd, j+padd, brStrongBILINEARDEMOSAICING));
            }
            if(i%2 == 1 && j%2 == 1){
                temp[1][i][j] = img.pixels[0][i+padd][j+padd];
                temp[2][i][j] = clipAndRound(pixConvolution(img, i+padd, j+padd, brWeakOrBILINEARDEMOSAICING));
                temp[0][i][j] = clipAndRound(pixConvolution(img, i+padd, j+padd, brWeakAndBILINEARDEMOSAICING));
            }
        }
    }
    //img.unPad();
    // img.pixels = std::move(temp);
    img.pixels = temp;
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
    std::vector<std::vector<std::vector<int>>> temp(img.RGB, std::vector<std::vector<int>>(img.height, std::vector<int>(img.width)));
    int padd = static_cast<int>(MHCDEMOSAICINGSIZE/2);
    img.padding(padd);

    for(auto i = 0; i < img.height; i++){
        for(auto j = 0; j < img.width; j++){
            if(i%2 == 0 && j%2 == 0){
                temp[1][i][j] = clipAndRound(img.pixels[0][i+padd][j+padd]);
                temp[2][i][j] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenEvenMHC));
                temp[0][i][j] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenOddMHC));
            }
            if(i%2 == 0 && j%2 == 1){
                temp[1][i][j] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, greenAtRedBlueMHC));
                temp[2][i][j] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, oppRedBlueMHC));
                temp[0][i][j] = clipAndRound(img.pixels[0][i+padd][j+padd]);
            }
            if(i%2 == 1 && j%2 == 0){
                temp[1][i][j] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, greenAtRedBlueMHC));
                temp[2][i][j] = clipAndRound(img.pixels[0][i+padd][j+padd]);
                temp[0][i][j] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, oppRedBlueMHC));
            }
            if(i%2 == 1 && j%2 == 1){
                temp[1][i][j] = clipAndRound(img.pixels[0][i+padd][j+padd]);
                temp[2][i][j] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenOddMHC));
                temp[0][i][j] = clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenEvenMHC));
            }
        }
    }
    img.pixels = std::move(temp);
}