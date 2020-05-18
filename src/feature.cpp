#include "feature.h"

extern std::vector<std::vector<double>> xSobel;
extern std::vector<std::vector<double>> ySobel;

sift::sift(const image& base)
    : dogSpace(base, 3, 1)
    , baseImg(base)
{
    scaleSpaceExtremaDetection(base);
    pointLocalization(base);
}

//Find extrema points at various scales of image via image pyramid -> DoG Pyramid
void sift::scaleSpaceExtremaDetection(const image& base)
{
    std::vector<image> temp;
    image tempImg(base);

    //generate gaussian subspace
    for(auto i = 0; i < dogSpace.pyramid.size(); ++i){ //iterate through octavies
        tempImg = dogSpace.pyramid[i];
        for(auto j = 0; j < MAXSCALESPACE; ++j){
            conv(tempImg, gaussFilter(5, SIGMA*pow(2.0, j))); //continually convolve img with gaussian to develop scale space
            temp.emplace_back(tempImg);
        }
        gaussPyramid.emplace_back(std::move(temp));
        temp.clear();
    }

    //generate DoG
    std::unique_ptr<image> imgPtr;
    for(auto i = 0; i < gaussPyramid.size(); ++i){
        for(auto j = 0; j < MAXSCALESPACE-1; ++j){
            imgPtr = sub(gaussPyramid[i][j], gaussPyramid[i][j+1]);
            temp.emplace_back(*imgPtr);
        }
        dog.emplace_back(std::move(temp));
        temp.clear();
    }

    // find preliminary features
    bool max = true;
    bool min = true; 

    int tempGaussVal = 2;
    for(auto i = 0; i < dog.size(); ++i){ // iterate through octaves
        if(i != 0){
            tempGaussVal = 1; //BUG, too many features coming from first scale, get rid of it with this
        }
        for(auto j = tempGaussVal; j < DOGSCALE-1; ++j){ // iterate through middle scales
            //iterate through all pixels
            for(auto l = 1; l < dog[i][0].height-1; ++l){
                for(auto m = 1; m < dog[i][0].width-1; ++m){
                    //compare nearby scales
                    for(auto k = -1; k < 2; ++k){
                        //compare nearby pixels 3x3
                        for(auto x = -1; x < 2; ++x){
                            for(auto y = -1; y < 2; ++y){
                                
                                if(x != 0 && y != 0 && k != 0){
                                    if(dog[i][j+k].pixels[0][l+x][m+y] >= dog[i][j].pixels[0][l][m]){
                                        max = false;
                                    }
                                    if(dog[i][j+k].pixels[0][l+x][m+y] <= dog[i][j].pixels[0][l][m]){
                                        min = false; 
                                    }
                                }

                            }
                        }
                    }
                    if(max or min){
                        if(dog[i][j].pixels[0][l][m] > 0.03 || dog[i][j].pixels[0][l][m] < 0.03){
                            features.emplace_back(std::make_tuple(static_cast<int>(l*pow(2.0, i-1)),static_cast<int>(m*pow(2.0, i-1)), dog[i][j].pixels[0][l][m]));
                        }
                    }
                    max = true;
                    min = true;
                }
            }
        }
    }
}

//Create Hessian Matrix at each point
// | Ixx Ixy |
// | Ixy Iyy |
void sift::pointLocalization(const image& base){
    image xx(base);
    conv(xx, xSobel);
    conv(xx, xSobel);
    image yy(base);
    conv(yy, ySobel);
    conv(yy, ySobel);
    image xy(base);
    conv(xy, xSobel);
    conv(xy, ySobel);

    double det;
    double trace;
    double responseRatio;

    size_t count = 0;
    for(auto& feat : features){
        trace = xx.pixels[0][std::get<0>(feat)][std::get<1>(feat)] + yy.pixels[0][std::get<0>(feat)][std::get<1>(feat)];
        det = xx.pixels[0][std::get<0>(feat)][std::get<1>(feat)] * yy.pixels[0][std::get<0>(feat)][std::get<1>(feat)] - pow(xy.pixels[0][std::get<0>(feat)][std::get<1>(feat)], 2);
        responseRatio = pow(trace, 2)/trace;
        if(det > 0){
            features.erase(features.begin()+count);
            std::cout << "pixel: " << std::get<0>(feat) << " " << std::get<1>(feat) << "\n";
            std::cout << "response ratio: " << responseRatio << "\n";
            std::cout << "Failed determinant\n";
        }
        else if (responseRatio < -10) {
            features.erase(features.begin()+count);
            std::cout << "pixel: " << std::get<0>(feat) << " " << std::get<1>(feat) << "\n";
            std::cout << "response ratio: " << responseRatio << "\n";
            std::cout << "Failed ratio\n";
        }
        else{
            std::cout << "pixel: " << std::get<0>(feat) << " " << std::get<1>(feat) << "\n";
            std::cout << "response ratio: " << responseRatio << "\n";
            std::cout << "Passed!\n";
        }
        ++count;
    }

}
void sift::pointOrientation(){}
void sift::pointDescriptor(){}



