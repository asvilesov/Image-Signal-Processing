#include "display.h"

void displayImg(image& img, const std::string& text){

    double val = 0;
    for(auto k = 0; k < img.channels; ++k){
        for(auto i = 0; i < img.height; ++i){
            for(auto j = 0; j < img.width; ++j){
                img.pixels[k][i][j] = img.clipAndRound(img.pixels[k][i][j]);
            }
        }
    }

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


//for drawing targets on image
void drawTarget(image& img, int x, int y){
    std::vector<std::vector<int>> targ;
    std::vector<int> temp;
    int size = 5;
    // for(auto i = 0; i < size; ++i){
    //     for(auto j = 0; j < size; ++j){
    //         temp.emplace_back(1);
    //     }
    //     targ.emplace_back(temp);
    //     temp.clear();
    // }
    if(x > size && x < img.height - size){
        if(y > size && y < img.width - size){
            for(auto i = 0; i < size; ++i){
                for(auto j = 0; j < size; ++j){
                    img.pixels[2][x-size/2+i][y-size/2+j] = 255;
                }
            }
        }
    }
}