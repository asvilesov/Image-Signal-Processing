#include <iostream>
#include <string>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "isp.h"

int main(int argc, char *argv[]){
    int height(300);
    int width(390);
    unsigned char input[height*width];
    
    
    char f[] = "../images/cat.raw";

    //input Single-CCD sensor input into image struct
    image img(f, height, width);
    //convert to openCV mat format for display
    cv::Mat img1(height, width, CV_8UC1, cv::Scalar(50));
    for(auto i = 0; i < height; i++){
        for(auto j = 0; j<width; j++){
			img1.at<uchar>(i, j) = img.pixels[0][i][j];
        }
    }

    //Uniform Filter
    image uniImg(img);
    uniformFilter(uniImg, 5);
    cv::Mat img2(height, width, CV_8UC1, cv::Scalar(50));
    for(auto i = 0; i < height; i++){
        for(auto j = 0; j<width; j++){
			img2.at<uchar>(i, j) = uniImg.pixels[0][i][j];
        }
    }

    //calculate PSNR
    float error = psnr(img, uniImg);

    //Display results
    cv::namedWindow( "Original", cv::WINDOW_AUTOSIZE);
    cv::imshow("Original", img1);
    cv::namedWindow("Uniform Filter with PSNR(dB) = " + std::to_string(error), cv::WINDOW_AUTOSIZE);
    cv::imshow("Uniform Filter with PSNR(dB) = " + std::to_string(error), img2);
    cv::waitKey(0);

}