//library for image signal processing

#include "isp.h"

imagePyramid::imagePyramid(image input, int minOctave, int maxOctave)
    :base(input)
{
    image halfBase(base);
    image twiceBase(base);
    auto blurBase(base);

    conv(blurBase, binomFilter(3));
    if(minOctave > MAXDEPTH){
        minOctave = MAXDEPTH;
    }

    //generaters octaves of larger resolution
    for(auto m = 0 ; m < maxOctave; ++m){
        blurBase = twiceBase;
        twiceBase = image(twiceBase.height*2, twiceBase.width*2, twiceBase.channels);
        conv(blurBase, binomFilter(3+2*m));
        
        for(auto i = 0; i < twiceBase.height; ++i){
            for(auto j = 0; j < twiceBase.width; ++j){
                for(auto k = 0; k < base.channels; ++k){
                    twiceBase.pixels[k][i][j] = blurBase.pixels[k][static_cast<int>(i/2)][static_cast<int>(j/2)];
                }
            }
        }

        gaussPyramid.push_back(blurBase);
        pyramid.push_back(twiceBase);
    }

    //add in original
    pyramid.push_back(base);

    //generates octaves of lower resolution
    for(auto m = 0 ; m < minOctave; ++m){
        if(base.height % 2 == 0 && base.width % 2 == 0){
            //create half image and gauss/laplace images
            blurBase = halfBase;
            halfBase = image(blurBase.height/2, blurBase.width/2, blurBase.channels);
            conv(blurBase, binomFilter(3+2*m)); //want to increase the  gaussian filter as we keep decreasing image size

            for(auto i = 0; i < halfBase.height; ++i){
                for(auto j = 0; j < halfBase.width; ++j){
                    for(auto k = 0; k < base.channels; ++k){
                        halfBase.pixels[k][i][j] = blurBase.pixels[k][2*i][2*j];
                    }
                }
            }
            gaussPyramid.push_back(blurBase);
            pyramid.push_back(halfBase);
        }  
        else{
            std::cout << "the image has either exceeded its max depth or does not have width/height as a base of 2\n";
        }
    }
}

//Sobel X and Y filters
std::vector<std::vector<double>> xSobel{{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};
std::vector<std::vector<double>> ySobel{{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

//Canny Edge Detection
void canny(image& img, int low, int high){
    auto tempX = img;
    auto tempY = img;
    tempX.MIN_DEV = -1000;
    tempY.MIN_DEV = -1000;
    tempX.MAX_DEV = +1000;
    tempY.MAX_DEV = +1000;
    img.MAX_DEV = 2000;
    conv(tempX, xSobel);
    conv(tempY, ySobel);
    std::vector<std::vector<double>> tempMag(std::vector<std::vector<double>>(tempX.height, std::vector<double>(tempX.width, 0.0)));
    std::vector<std::vector<double>> tempAng(std::vector<std::vector<double>>(tempX.height, std::vector<double>(tempX.width, 0.001)));
    for(auto i = 0; i < img.height; ++i){
        for(auto j = 0; j < img.width; ++j){
            tempMag[i][j] = img.clipAndRound(std::sqrt(pow(tempX.pixels[0][i][j], 2) + pow(tempY.pixels[0][i][j], 2)));
            if(tempX.pixels[0][i][j] == 0 && tempY.pixels[0][i][j] > 0){
                tempAng[i][j] = 90;
            }
            else if(tempX.pixels[0][i][j] == 0 && tempY.pixels[0][i][j] < 0){
                tempAng[i][j] = -90;
            }
            else if(tempX.pixels[0][i][j] == 0 && tempY.pixels[0][i][j] == 0){
                tempAng[i][j] = 0.00;
            }
            else{
                tempAng[i][j] = std::atan(tempY.pixels[0][i][j]/(tempX.pixels[0][i][j]))*180/M_PI;
            }

            if(tempAng[i][j] < 0.00){
                tempAng[i][j] +=360.0;
            }
            //std::cout << tempAng[i][j] << " ";
        }
    }
    auto nonMaxSupp(tempMag);
    for(auto i = 1; i < img.height-1; ++i){
        for(auto j = 1; j < img.width-1; ++j){
            if((tempAng[i][j] > 360-22.5 || tempAng[i][j] < 22.5) || (tempAng[i][j] > 180-22.5 && tempAng[i][j] < 180+22.5)){
                if(tempMag[i+1][j] > tempMag[i][j] || tempMag[i-1][j] > tempMag[i][j]){
                    nonMaxSupp[i][j] = 0;
                }
            }
            else if((tempAng[i][j] > 22.5 && tempAng[i][j] < 67.5) || (tempAng[i][j] > 225-22.5 && tempAng[i][j] < 225+22.5)){
                if(tempMag[i+1][j+1] > tempMag[i][j] || tempMag[i-1][j-1] > tempMag[i][j]){
                    nonMaxSupp[i][j] = 0;
                }
            }
            else if((tempAng[i][j] < 90+22.5 && tempAng[i][j]) > 90-22.5 || (tempAng[i][j] > 270-22.5 && tempAng[i][j] < 270+22.5)){
                if(tempMag[i][j+1] > tempMag[i][j] || tempMag[i][j-1] > tempMag[i][j]){
                    nonMaxSupp[i][j] = 0;
                }
            }
            else if((tempAng[i][j] < 305+22.5 && tempAng[i][j] > 305-22.5) || (tempAng[i][j] > 135-22.5 && tempAng[i][j] < 135+22.5)){
                if(tempMag[i-1][j+1] > tempMag[i][j] || tempMag[i+1][j-1] > tempMag[i][j]){
                    nonMaxSupp[i][j] = 0;
                }
            }
        }
    }

    img.MAX_DEV = 256;
    for(auto i = 0; i < img.height; ++i){
        for(auto j = 0; j < img.width; ++j){
            img.pixels[0][i][j] = img.clipAndRound(nonMaxSupp[i][j]);
        }
    }
}

//Sobel Edge Detection
void sobelFilter(image& img){
    auto tempX = img;
    auto tempY = img;
    conv(tempX, xSobel);
    conv(tempY, ySobel);
    for(auto i = 0; i < img.height; ++i){
        for(auto j = 0; j < img.width; ++j){
            img.pixels[0][i][j] = img.clipAndRound(std::sqrt(pow(tempX.pixels[0][i][j], 2) + pow(tempY.pixels[0][i][j], 2)));
        }
    }
}

//Identity Filter
std::vector<std::vector<float>> identityKernel(const int& size, const float& gain = 1.0){
    std::vector<std::vector<float>> temp(std::vector<std::vector<float>>(size, std::vector<float>(size, 0.0)));
    int middle = static_cast<int>((size - 1)/2);
    temp[middle][middle] = gain;
    return temp;
}

//1D Binomial Filter Generator
std::vector<int> binomialGenerator(int depth){
    if(depth == 1){
        return {1};
    }
    --depth;
    std::vector<int> basis{1,1};
    std::vector<int> newFilter(basis);
    std::vector<int> temp;
    for(auto i = 1; i < depth; ++i){
        temp.push_back(1);
        for(auto j = 0; j < newFilter.size()-1; ++j){
            temp.push_back(newFilter[j]+newFilter[j+1]);
        }
        temp.push_back(1);
        newFilter = temp;
        temp = {};
    }
    return newFilter;
}

//Binomial Filter (approx Gauss)
std::vector<std::vector<double>> binomFilter(const int& size){
    std::vector<std::vector<double>> temp(std::vector<std::vector<double>>(size, std::vector<double>(size)));
    auto binomFilt(binomialGenerator(size));
    double sum = 0;
    for(auto i = 0; i < size; ++i){
        for(auto j = 0; j <size; ++j){
            temp[i][j] = binomFilt[i]*binomFilt[j];
            sum += temp[i][j];
        }
    }
    for(auto i = 0; i < size; ++i){
        for(auto j = 0; j <size; ++j){
            temp[i][j] /= sum;
        }
    }
    return temp;
}

// Gaussian Function
double gauss(const int& x, const int& y, const float& std){
    auto num = std::exp(-(std::pow(x,2)+std::pow(y,2))/(2.0*std::pow(std,2)));
    return num/(std::pow(2.0*M_PI*std::pow(std,2), 0.5));
}

// Gaussian Filter
std::vector<std::vector<double>> gaussFilter(const int& size, const double& std){
    std::vector<std::vector<double>> temp(std::vector<std::vector<double>>(size, std::vector<double>(size)));
    int convSize = (size-1)/2;
    double sum = 0;
    double tempElem = 0;
    for(auto i = 0; i < size; ++i){
        for(auto j = 0; j < size; ++j){
            tempElem = gauss(i-convSize, j-convSize, std);
            temp[i][j] = tempElem;
            sum += tempElem;
        }
    }
    for(auto i = 0; i < size; ++i){
        for(auto j = 0; j < size; ++j){
            temp[i][j] /= sum;
        }
    }
    return temp;
}

//sharpening an image with uniform kernel
std::vector<std::vector<float>> sharpenFilter(const int&size){
    return subtractFilters<float>(identityKernel(size, 2), uniformKern(size));
}

//creating a uniform convolution kernel
std::vector<std::vector<float>> uniformKern(const int& size){
    float elem = 1.0/static_cast<float>(size*size);
    return std::vector<std::vector<float>>(size, std::vector<float> (size, elem));
}

//uniform lowpass filter
void uniformFilter(image& img, const int&size){
    conv(img, uniformKern(size));
}

//histogram of an image's pixels (not normalized) (assumes one channel)
std::vector<double> imageHistogram(const std::vector<std::vector<std::vector<double>>>& img){
    std::vector<double> histogram(256, 0);
    for(auto& i : img[0]){
        for(auto& j : i){
            ++histogram[j];
        }
    }
    return histogram;
}

//CDF of the histogram of an image (not normalized)
std::vector<double> imageCDF(const std::vector<std::vector<std::vector<double>>>& img){
    auto histogramCdf(imageHistogram(img));
    int temp = 0;
    for(auto& i : histogramCdf){
        i+=temp;
        temp = i;
    }
    return histogramCdf;
}

//Mean Square Error between two images
float mse(const image& imgX, const image& imgY){
    float meanSquareError = 0;
    float maxError = 0;
    float curError = 0;
    for(auto k = 0; k < imgX.channels; ++k){
        for(auto i = 0; i < imgX.height; ++i){
            for(auto j = 0; j < imgX.width; ++j){
                curError = imgX.pixels[k][i][j] - imgY.pixels[k][i][j];
                meanSquareError += pow(curError, 2);
            }
        }
    }
    meanSquareError /= (imgX.channels*imgX.height*imgX.width);
}

//PSNR - Peak Signal-to-noise Ratio
float psnr(const image& imgX, const image& imgY){
    float mseXY = mse(imgX, imgY);
    return 10*log10(static_cast<float>(pow(imgX.MAX_DEV, 2))/mseXY);
}

//passing in a one channel image, default is linear ***assumes one channel
void histogramEqualization(std::vector<std::vector<std::vector<double>>>& img){
    auto hist = imageCDF(img);
    std::vector<double> histPdf(256);
    double numPixels = img[0].size()*img[0][0].size();
    size_t count = 0;
    std::for_each(histPdf.begin(), histPdf.end(), [&numPixels, &hist, &count](double& a){
        a = hist[count]/numPixels;
        a*=255;
        ++count;
    });
    for(auto i = 0; i < img[0].size(); ++i){
        for(auto j = 0; j < img[0][0].size(); ++j){
            img[0][i][j] = static_cast<int>(histPdf[img[0][i][j]]);
        }
    }
}

const int BILINEARDEMOSAICINGSIZE = 3;

const float brStrongBILINEARDEMOSAICING[3][3] = {
    {0.25,0, 0.25},
    {0,   0, 0},
    {0.25,0, 0.25}};

const float greenStrongBILINEARDEMOSAICING[3][3] = {
    {0,    0.25,0},
    {0.25, 0,   0.25},
    {0,    0.25,0}};

const float brWeakAndBILINEARDEMOSAICING[3][3] = {
    {0,0.5,0},
    {0,0,0},
    {0,0.5,0}};

const float brWeakOrBILINEARDEMOSAICING[3][3] = {
    {0,0,0},
    {0.5,0,0.5},
    {0,0,0}};

const float IDENTITY[3][3] = {
    {0,0,0},
    {0,1,0},
    {0,0,0}};

void bilinearDemosaicing(image& img){
    std::vector<std::vector<std::vector<double>>> temp(img.RGB, std::vector<std::vector<double>>(img.height, std::vector<double>(img.width)));
    int padd = static_cast<int>(BILINEARDEMOSAICINGSIZE/2);
    img.padding(padd);

    for(auto i = 0; i < img.height; i++){
        for(auto j = 0; j < img.width; j++){
            if(i%2 == 0 && j%2 == 0){
                temp[1][i][j] = img.pixels[0][i+padd][j+padd];
                temp[2][i][j] = img.clipAndRound(pixConvolution(img, i+padd, j+padd, brWeakAndBILINEARDEMOSAICING));
                temp[0][i][j] = img.clipAndRound(pixConvolution(img, i+padd, j+padd, brWeakOrBILINEARDEMOSAICING));
            }
            if(i%2 == 0 && j%2 == 1){
                temp[1][i][j] = img.clipAndRound(pixConvolution(img, i+padd, j+padd, greenStrongBILINEARDEMOSAICING));
                temp[2][i][j] = img.clipAndRound(pixConvolution(img, i+padd, j+padd, brStrongBILINEARDEMOSAICING));
                temp[0][i][j] = img.pixels[0][i+padd][j+padd];
            }
            if(i%2 == 1 && j%2 == 0){
                temp[1][i][j] = img.clipAndRound(pixConvolution(img, i+padd, j+padd, greenStrongBILINEARDEMOSAICING));
                temp[2][i][j] = img.pixels[0][i+padd][j+padd];
                temp[0][i][j] = img.clipAndRound(pixConvolution(img, i+padd, j+padd, brStrongBILINEARDEMOSAICING));
            }
            if(i%2 == 1 && j%2 == 1){
                temp[1][i][j] = img.pixels[0][i+padd][j+padd];
                temp[2][i][j] = img.clipAndRound(pixConvolution(img, i+padd, j+padd, brWeakOrBILINEARDEMOSAICING));
                temp[0][i][j] = img.clipAndRound(pixConvolution(img, i+padd, j+padd, brWeakAndBILINEARDEMOSAICING));
            }
        }
    }
    img.pixels = std::move(temp);
    img.channels = 3;
    img.color = RGB;
}

const int MHCDEMOSAICINGSIZE = 5;

const float  greenAtRedBlueMHC[5][5] = {
    {0, 0, -1, 0, 0},
    {0, 0, +2, 0, 0},
    {-1, +2, +4, +2, -1},
    {0, 0, +2, 0, 0},
    {0, 0, -1, 0, 0}
};

const float redBlueAtGreenOddMHC[5][5] = {
    {0, 0, +1/2, 0, 0},
    {0, -1, 0, -1, 0},
    {-1, +4, +5, +4, -1},
    {0, -1, 0, -1, 0},
    {0, 0, +1/2, 0, 0}
};

const float redBlueAtGreenEvenMHC[5][5] = {
    {0, 0, -1, 0, 0},
    {0, -1, 4, -1, 0},
    {+1/2, 0, +5, 0, +1/2},
    {0, -1, 4, -1, 0},
    {0, 0, -1, 0, 0}
};

const float oppRedBlueMHC[5][5] = {
    {0, 0, -3/2, 0, 0},
    {0, 2, 0, 2, 0},
    {-3/2, 0, +6, 0, -3/2},
    {0, 2, 0, 2, 0},
    {0, 0, -3/2, 0, 0}
};

void demosaicingMHC(image& img){
    std::vector<std::vector<std::vector<double>>> temp(img.RGB, std::vector<std::vector<double>>(img.height, std::vector<double>(img.width)));
    int padd = static_cast<int>(MHCDEMOSAICINGSIZE/2);
    img.padding(padd);

    for(auto i = 0; i < img.height; i++){
        for(auto j = 0; j < img.width; j++){
            if(i%2 == 0 && j%2 == 0){
                temp[1][i][j] = img.clipAndRound(img.pixels[0][i+padd][j+padd]);
                temp[2][i][j] = img.clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenEvenMHC));
                temp[0][i][j] = img.clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenOddMHC));
            }
            if(i%2 == 0 && j%2 == 1){
                temp[1][i][j] = img.clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, greenAtRedBlueMHC));
                temp[2][i][j] = img.clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, oppRedBlueMHC));
                temp[0][i][j] = img.clipAndRound(img.pixels[0][i+padd][j+padd]);
            }
            if(i%2 == 1 && j%2 == 0){
                temp[1][i][j] = img.clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, greenAtRedBlueMHC));
                temp[2][i][j] = img.clipAndRound(img.pixels[0][i+padd][j+padd]);
                temp[0][i][j] = img.clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, oppRedBlueMHC));
            }
            if(i%2 == 1 && j%2 == 1){
                temp[1][i][j] = img.clipAndRound(img.pixels[0][i+padd][j+padd]);
                temp[2][i][j] = img.clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenOddMHC));
                temp[0][i][j] = img.clipAndRound(0.125*pixConvolution(img, i+padd, j+padd, redBlueAtGreenEvenMHC));
            }
        }
    }
    img.pixels = std::move(temp);
    img.channels = 3;
    img.color = RGB;
}