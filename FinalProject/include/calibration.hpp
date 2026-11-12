#pragma once

#include <opencv2/core.hpp>
#include <string>

using namespace cv;
using namespace std;

struct Calibration {
    Mat cameraMatrix;
    Mat distCoeffs;
};

Calibration loadCalibration(string filename);
