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
    char f[] = "../images/cat.raw";

    //input Single-CCD sensor input into image struct
    image img1(f, height, width);
    //convert to openCV mat format for display
    cv::Mat img(height, width, CV_8UC1, cv::Scalar(50));
    for(auto i = 0; i < height; i++){
        for(auto j = 0; j<width; j++){
			img.at<uchar>(i, j) = img1.pixels[0][i][j];
        }
    }

    //bilinear Demosaicing reconstruction
    bilinearDemosaicing(img1);
    unsigned char * ptr = nullptr;
    cv::Mat bilin_img(img1.height, img1.width, CV_8UC3, cv::Scalar(0,0,0));
    for(auto i = 0; i < img1.height; i++){
        for(auto j = 0; j<img1.width; j++){
            ptr = bilin_img.ptr(i, j);
			ptr[0] = img1.pixels[0][i][j];
            ptr[1] = img1.pixels[1][i][j];
            ptr[2] = img1.pixels[2][i][j];
        }
    }

    //MHC Demosaicing
    image img2(f,height,width);
    demosaicingMHC(img2);
    ptr = nullptr;
    cv::Mat mhc_img(img2.height, img2.width, CV_8UC3, cv::Scalar(0,0,0));
    for(auto i = 0; i < img2.height; i++){
        for(auto j = 0; j<img2.width; j++){
            ptr = mhc_img.ptr(i, j);
			ptr[0] = img2.pixels[0][i][j];
            ptr[1] = img2.pixels[1][i][j];
            ptr[2] = img2.pixels[2][i][j];
        }
    }

    //Display results
    cv::namedWindow( "Single-CCD sensor covered by a CFA with Bayer Pattern", cv::WINDOW_AUTOSIZE);
    cv::imshow("Single-CCD sensor covered by a CFA with Bayer Pattern", img);
    cv::namedWindow( "Bilinear Demosaicing", cv::WINDOW_AUTOSIZE);
    cv::imshow("Bilinear Demosaicing", bilin_img);
    cv::namedWindow( "MHC Demosaicing", cv::WINDOW_AUTOSIZE);
    cv::imshow("MHC Demosaicing", mhc_img);
    cv::waitKey(0);


}