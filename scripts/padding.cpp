#include <iostream>
#include "isp.h"
#include "display.h"

int main(int argc, char *argv[]){
    int height(300);
    int width(390);
    unsigned char input[height*width];
    int padding = 50;
    char f[] = "../images/cat.raw";

    //input Single-CCD sensor input into image struct
    image img1(f, height, width);
    displayImg(img1, std::string("Original"));

    img1.padding(padding);
    displayImg(img1, std::string("Reflective Padding"));

    cv::waitKey(0);

    return 0;
}