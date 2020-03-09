#include "isp.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>

int main(int argc, char *argv[]){
    int height(400);
    int width(400);
    unsigned char input[height*width];
    int padding = 50;

    if(argc == 2){
        char f[] = "../images/rose_dark.raw";
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
        char f[] = "../images/rose_dark.raw";
        char* filename = f;
        read(filename, input, height * width);
    }
    image img1(input, height, width);

    std::vector<double> y(imageCDF(img1.pixels));
    cv::Mat display;
    cv::Mat y_data(y);
    cv::Ptr<cv::plot::Plot2d> plotter = cv::plot::createPlot2d(y_data);
    plotter->render(display);
    cv::imshow("Histogram Before Equalization", display);

    //input Single-CCD sensor input into image struct
    cv::Mat img2(height, width, CV_8UC1, cv::Scalar(50));
    for(auto i = 0; i < height; i++){
        for(auto j = 0; j<width; j++){
			img2.at<uchar>(i, j) = img1.pixels[i][j];
        }
    }

    histogramEqualization(img1.pixels);
    std::vector<double> x(imageCDF(img1.pixels));
    cv::Mat display2;
    cv::Mat x_data(x);
    cv::Ptr<cv::plot::Plot2d> plotter2 = cv::plot::createPlot2d(x_data);
    plotter2->render(display2);
    cv::imshow("Histogram After Equalization", display2);
    cv::Mat img(height, width, CV_8UC1, cv::Scalar(50));
    for(auto i = 0; i < height; i++){
        for(auto j = 0; j<width; j++){
			img.at<uchar>(i, j) = img1.pixels[i][j];
        }
    }

    //Display results
    cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Original Image", img2);
    cv::namedWindow("Histogram Linear Equalization", cv::WINDOW_AUTOSIZE);
    cv::imshow("Histogram Linear Equalization", img);
    cv::waitKey(0);

    return 0;
}