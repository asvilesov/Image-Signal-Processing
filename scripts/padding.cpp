#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "isp.h"

int main(int argc, char *argv[]){
    int height(300);
    int width(390);
    unsigned char input[height*width];
    int padding = 50;

    if(argc == 2){
        char f[] = "../images/cat.raw";
        char* filename = f;
        std::string pad(argv[1]);
        padding = std::stoi(pad);
        read(filename, input, height * width);
    }
    if(argc == 3){
        char* filename = argv[1];
        std::string pad(argv[2]);
        padding = std::stoi(pad);
        read(filename, input, height * width);
    }
    else{
        char f[] = "../images/cat.raw";
        char* filename = f;
        read(filename, input, height * width);
    }

    //input Single-CCD sensor input into image struct
    image img1(input, height, width);
    cv::Mat img2(height, width, CV_8UC1, cv::Scalar(50));
    for(auto i = 0; i < height; i++){
        for(auto j = 0; j<width; j++){
			img2.at<uchar>(i, j) = img1.pixels[i][j];
        }
    }

    img1.padding(padding);
    cv::Mat img(height+padding, width+padding, CV_8UC1, cv::Scalar(50));
    for(auto i = 0; i < height+padding; i++){
        for(auto j = 0; j<width+padding; j++){
			img.at<uchar>(i, j) = img1.pixels[i][j];
        }
    }

    //Display results
    cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Original Image", img2);
    cv::namedWindow("Reflective Padding", cv::WINDOW_AUTOSIZE);
    cv::imshow("Reflective Padding", img);
    cv::waitKey(0);

    return 0;
}