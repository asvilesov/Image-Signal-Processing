#pragma once

#include "image.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>

void displayImg(image& img, const std::string& text);

void drawTarget(image& img, int x, int y);
