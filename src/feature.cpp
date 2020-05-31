#include "feature.h"

extern std::vector<std::vector<double>> xSobel;
extern std::vector<std::vector<double>> ySobel;

sift::sift(const image& base)
    : scaleSpace(base, 3, 1)
    , baseImg(base)
{
    scaleSpaceExtremaDetection(base);
    keepContrast(base);
    pointOrientation();
}

//Find extrema points at various scales of image via image pyramid -> DoG Pyramid
void sift::scaleSpaceExtremaDetection(const image& base)
{
    std::vector<image> temp;
    image tempImg(base);

    //generate gaussian subspace
    for(auto i = 0; i < scaleSpace.pyramid.size(); ++i){ //iterate through octavies
        tempImg = scaleSpace.pyramid[i];
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
                        if(dog[i][j].pixels[0][l][m] > 0.5 || dog[i][j].pixels[0][l][m] < -0.5){
                            std::pair<double, double> displacement{0,0};
                            double x = l;
                            double y = m;
                            if(dog[i][j].height - 2 > l && dog[i][j].width - 2 > m && l > 1 && m > 1){
                                displacement = pointLocalization(dog[i][j], l, m);
                            }
                            x+=displacement.first;
                            y+=displacement.second;
                            std::cout << "x = " << static_cast<int>(x*pow(2.0, i-1)) << " y= " << static_cast<int>(y*pow(2.0, i-1)) << "\n";
                            std::cout <<"Value: " << dog[i][j].pixels[0][l][m] << "\n";
                            std::cout <<"Point Displacement: " << displacement.first << " " << displacement.second << "\n";
                            features.emplace_back(std::make_tuple(static_cast<int>(x*pow(2.0, i-1)),static_cast<int>(y*pow(2.0, i-1)), dog[i][j].pixels[0][l][m]));
                        }
                    }
                    max = true;
                    min = true;
                }
            }
        }
    }
}

//Calculates subpixel resolution through the first three terms of a Taylor Approximation
// Dsub(x) = D(x) + D'(x)T*x + 1/2*xT*D''(x)*x
std::pair<double, double> sift::pointLocalization(const image& img, int x, int y){
    int size = 2;

    std::vector<std::vector<double>> xPatch(5, std::vector<double>(5));
    std::vector<std::vector<double>> yPatch(5, std::vector<double>(5));
    std::vector<std::vector<double>> xyPatch(5, std::vector<double>(5));

    for(auto i = -size; i <=size; ++i){
        for(auto j = -size; j <=size; ++j){
            xPatch[i+size][j+size] = pixConvolution(img.pixels, x+i, y+j, xSobel);
            yPatch[i+size][j+size] = pixConvolution(img.pixels, x+i, y+j, ySobel);
        }
    }
    xyPatch = xPatch;

    double dx = xPatch[2][2];
    double dy = yPatch[2][2];
    
    double dxx = pixConvolution(xPatch, 2, 2, xSobel);
    double dyy = pixConvolution(yPatch, 2, 2, ySobel);
    double dxy = pixConvolution(xPatch, 2, 2, ySobel);
    
    Eigen::MatrixXd m(2, 2);
    m << dxx, dxy,
        dxy, dyy;
    Eigen::Vector2d v({dx, dy});
    auto delta = m.inverse()*v;
    return std::pair<double, double>({delta[0], -delta[1]}); //add negative since the y-axis is inverted in images
}

//Create Hessian Matrix at each point
// | Ixx Ixy |
// | Ixy Iyy |
//Isolate high contrast points
void sift::keepContrast(const image& base){
    image xx(base);
    conv(xx, xSobel);
    xGradient = xx; //save gradient calculations for later
    conv(xx, xSobel);
    image yy(base);
    conv(yy, ySobel);
    yGradient = yy;
    conv(yy, ySobel);
    image xy(base);
    conv(xy, xSobel);
    conv(xy, ySobel);

    double det;
    double trace;
    double responseRatio;

    double responseRatioTest = 10;

    //For loop may be incorrect with the deleted elements
    size_t count = 0;
    for(auto& feat : features){
        trace = xx.pixels[0][std::get<0>(feat)][std::get<1>(feat)] + yy.pixels[0][std::get<0>(feat)][std::get<1>(feat)];
        det = xx.pixels[0][std::get<0>(feat)][std::get<1>(feat)] * yy.pixels[0][std::get<0>(feat)][std::get<1>(feat)] - pow(xy.pixels[0][std::get<0>(feat)][std::get<1>(feat)], 2);
        responseRatio = pow(trace, 2)/det;
        if(det < 0){
            features.erase(features.begin()+count);
            std::cout << "pixel: " << std::get<0>(feat) << " " << std::get<1>(feat) << "\n";
            std::cout << "response ratio: " << responseRatio << "\n";
            std::cout << "Failed determinant\n";
        }
        else if (responseRatio > (responseRatioTest+1)*(responseRatioTest+1)/responseRatioTest) {
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

//Descriptor representation through histogram of neighboring gradient and angles with a gaussian weighting function
// 16x16 sample array condensed to 4x4 descriptor with histograms = 4x4x8 = 128 element feature vector
void sift::pointOrientation(){
    //pick right/below as adjusted center, we cannot have center since we need to use even sub regions
    auto weightFunc = gaussFilter(17, 2);

    std::vector<double> featureVec;
    std::vector<double> histogram(8);
    std::pair<double, double> magPhase;
    int histAngle; 
    for(auto i : features){
        //iterate through 16 sub regions
        for(auto j = -2; j <= 2; ++j){
            for(auto k = -2; k <= 2; ++k){
                //SUB Region
                for(auto x = 0; x < 4; ++x){
                    for(auto y = 0; y < 4; ++y){
                        magPhase = magDir(xGradient.pixels[0][std::get<0>(i)+4*j+x][std::get<1>(i)+4*k+y], yGradient.pixels[0][std::get<0>(i)+4*j+x][std::get<1>(i)+4*k+y]);
                        magPhase.first *= weightFunc[8+4*j+x][8+4*k+y]; //apply Gauss weighting function so outer gradients have less affect

                        if(magPhase.second < 0){
                            magPhase.second += 360.00;
                        }
                        magPhase.second += 22.5; // shifting rotation axis so a value that qualifies as 0 degrees is between 338.5 to 22.5 degrees is now between 0 and 45
                        histAngle = static_cast<int>(round(magPhase.second/45));
                        if(histAngle == 8){
                            histogram[0] += magPhase.first;
                        }
                        else{
                            histogram[histAngle] += magPhase.first;
                        }

                        featureVec.insert(featureVec.end(), histogram.begin(), histogram.end());
                        for(auto& z : histogram) z = 0;
                    }
                }
            }
        }
        featureDescription.emplace_back(featureVec);
        featureVec.clear();
    }
}
void sift::pointDescriptor(){}



