#include "isp.h"
#include "display.h"

int main(int argc, char *argv[]){
    int height(400);
    int width(400);
    unsigned char input[height*width];
    char f[] = "../images/rose_dark.raw";
    
    image img1(f, height, width);

    std::vector<double> y(imageCDF(img1.pixels));
    cv::Mat display;
    cv::Mat y_data(y);
    cv::Ptr<cv::plot::Plot2d> plotter = cv::plot::createPlot2d(y_data);
    plotter->render(display);
    cv::imshow("Histogram Before Equalization", display);
    //input Single-CCD sensor input into image struct
    displayImg(img1, std::string("Original Image"));

    histogramEqualization(img1.pixels);
    std::vector<double> x(imageCDF(img1.pixels));
    cv::Mat display2;
    cv::Mat x_data(x);
    cv::Ptr<cv::plot::Plot2d> plotter2 = cv::plot::createPlot2d(x_data);
    plotter2->render(display2);
    cv::imshow("Histogram After Equalization", display2);
    displayImg(img1, std::string("Histogram Linear Equalization"));

    cv::waitKey(0);

    return 0;
}