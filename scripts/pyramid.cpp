#include <iostream>
#include "isp.h"
#include "display.h"
#include <time.h>
#include <chrono>
#include <thread>

int main(int argc, char *argv[]){

    std::string a(std::string("../images/lena.bmp"));
    image img(a);
    img.grayScale();

    displayImg(img, std::string("test"));

    imagePyramid res(img, 4, 1);
    
    for(auto i = 0 ; i < res.pyramid.size(); ++i){
        displayImg(res.pyramid[i], std::string(std::to_string(i)));
    }

    cv::waitKey(0); // Wait for a keystroke in the window
    return 0;
}