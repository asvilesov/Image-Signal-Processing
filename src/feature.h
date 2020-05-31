#pragma once

#include "isp.h"
#include "image.h"
#include "isp.h"
#include "eigen3/Eigen/Dense"

//SIFT object for storing extraced features from image
struct sift
{
    imagePyramid scaleSpace;
    std::vector<std::vector<image>> gaussPyramid; //TODO
    std::vector<std::vector<image>> dog; //arr of DoG images at each space: according to SIFT paper, 4 octaves (including original) with 4 spaces in each one
    image baseImg;
    image xGradient;
    image yGradient;
    std::vector<std::tuple<int, int, int>> features; //all located features (x,y,value)
    std::vector<std::vector<double>> featureDescription; //orientation histogram of each keypoint - 128 element feature vector for each point
    const double SIGMA = 1.8; //std for gauss ops
    const int MAXSCALESPACE = 6; //max number of times we convolve gauss with img in octave
    const int DOGSCALE = 5; //if scale is at 5, then we have three sampled scales in each octave

    sift(const image& base);
    void scaleSpaceExtremaDetection(const image& base);
    std::pair<double, double> pointLocalization(const image& img, int x, int y);
    void keepContrast(const image& base);
    void pointOrientation();
    void pointDescriptor();
};


