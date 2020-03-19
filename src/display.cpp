#include "display.h"

void displayImg(const image& img, const std::string& text){
    cv::Mat cvImg;
    if(img.channels == 1){
        cvImg = cv::Mat(img.height, img.width, CV_8UC1, cv::Scalar(50));
        for(auto i = 0; i < img.height; i++){
            for(auto j = 0; j< img.width; j++){
                cvImg.at<uchar>(i, j) = img.pixels[0][i][j];
            }
        }
    }
    else if(img.channels == 3){
        unsigned char* ptr = nullptr;
        cvImg = cv::Mat(img.height, img.width, CV_8UC3, cv::Scalar(0,0,0));
        for(auto i = 0; i < img.height; i++){
            for(auto j = 0; j<img.width; j++){
                ptr = cvImg.ptr(i, j);
                ptr[0] = img.pixels[0][i][j];
                ptr[1] = img.pixels[1][i][j];
                ptr[2] = img.pixels[2][i][j];
            }
        }
    }
    cv::namedWindow(text, cv::WINDOW_AUTOSIZE);
    cv::imshow(text, cvImg);
}