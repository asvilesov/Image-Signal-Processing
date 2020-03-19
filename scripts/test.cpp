#include <iostream>
#include "isp.h"
#include "display.h"
#include <time.h>
#include <chrono>
#include <thread>

int main(int argc, char *argv[]){

    std::string a(std::string("../images/monaLisa.jpg"));
    image img(a);
    displayImg(img, std::string("test"));
    cv::waitKey(1000);
    img.grayScale();
    displayImg(img, std::string("test"));
    cv::waitKey(1000);
    uniformFilter(img, 5);
    displayImg(img, std::string("test"));
    cv::waitKey(1000);
    sobelFilter(img);
    displayImg(img, std::string("test"));

    cv::waitKey(0);                                          // Wait for a keystroke in the window
    return 0;


}