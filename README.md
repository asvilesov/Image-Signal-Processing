# Image Signal Processing

C++ Implementations of ISP 

## Installation
After cloning the repo

```bash
mkdir build
cd build
cmake ..
make
```
The folder 'bin' will contain all executables.
## Navigation
src - contains source files \
scripts - contains files to be compiled into executables (examples) \
images - contains images to process \
build - contains cmake \
bin - exectubles


## Usage (e.g.)

```bash
./hist
```
This command will run histogram equalization on an image

## Contents
### Image Processing
Demosaicing -> MHC, Bilinear
Edge Detection -> Canny, Sobel
Image Pyramids -> Gaussian+Laplace
Histogram Equalization
SNR
Filters -> Binomial, Gaussian, Uniform, Sharpen

### Feature Detection and Matching
SIFT (todo)

### Compression
JPEG (todo)



Please make sure to update tests as appropriate.
