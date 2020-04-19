#include "isp.h"
#include "image.h"

//SIFT object for storing extraced features from image
struct sift
{
    imagePyramid dogSpace;
    std::vector<std::vector<image>> gaussPyramid;
    std::vector<std::vector<image>> dog; //arr of DoG images at each space: according to SIFT paper, 4 octaves (including original) with 4 spaces in each one
    image baseImg;
    std::vector<std::tuple<int, int, int>> features; //all located features
    const double SIGMA = 1.8; //std for gauss ops
    const int MAXSCALESPACE = 6; //max number of times we convolve gauss with img in octave
    const int DOGSCALE = 5; //if scale is at 5, then we have three sampled scales in each octave

    sift(const image& base);
};


