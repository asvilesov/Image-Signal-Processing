#include <iostream>
#include <string>
#include "isp.h"
#include "display.h"

int main(int argc, char *argv[]){
    int height(300);
    int width(390);
    unsigned char input[height*width];
    
    char f[] = "../images/cat.raw";

    //input Single-CCD sensor input into image struct
    image img(f, height, width);
    //convert to openCV mat format for display
    displayImg(img, std::string("Original"));

    //Uniform Filter
    image uniImg(img);
    uniformFilter(uniImg, 5);
    //calculate PSNR
    float error = psnr(img, uniImg);
    displayImg(uniImg, std::string("Uniform Filter with PSNR(dB) = " + std::to_string(error)));

    cv::waitKey(0);

}