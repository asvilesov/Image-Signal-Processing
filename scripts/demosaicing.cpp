#include <iostream>
#include "isp.h"
#include "display.h"

int main(int argc, char *argv[]){
    int height(300);
    int width(390);
    unsigned char input[height*width];
    char f[] = "../images/cat.raw";

    //input Single-CCD sensor input into image struct
    image img1(f, height, width);
    //convert to openCV mat format for display
    displayImg(img1, std::string("Single-CCD sensor covered by a CFA with Bayer Pattern"));

    //bilinear Demosaicing reconstruction
    bilinearDemosaicing(img1);
    displayImg(img1, std::string("Bilinear Demosaicing"));

    //MHC Demosaicing
    image img2(f,height,width);
    demosaicingMHC(img2);
    displayImg(img2, std::string("MHC Demosaicing"));
    
    cv::waitKey(0);


}