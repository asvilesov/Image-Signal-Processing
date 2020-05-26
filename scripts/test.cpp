#include <iostream>
#include "isp.h"
#include "display.h"
#include "feature.h"
#include <time.h>
#include <chrono>
#include <thread>

int main(int argc, char *argv[]){

    std::string a(std::string("../images/cath.jpg"));
    image img(a);
    image img2(a);
    img.grayScale();

    sift res(img);
    
    ////Display DoG
    // for(auto i = 0 ; i < res.dog[0].size(); ++i){
    //     displayImg(res.dog[1][i], std::string(std::to_string(i)));
    // }
    // std::cout << res.features.size() << "\n";

    // Original Image
    // displayImg(res.baseImg, "really");

    for(auto& i : res.features){
        drawTarget(img2, std::get<0>(i), std::get<1>(i));
    }
    displayImg(img2, std::string("r > 10 "));

    cv::waitKey(0); // Wait for a keystroke in the window
    return 0;
}