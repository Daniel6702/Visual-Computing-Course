
#pragma once

#include <opencv2/core.hpp>
#include <string>

using namespace std;
using namespace cv;

struct Calibration
{
    Mat cameraMatrix;
    Mat distCoeffs;
};

Calibration calibrate(string imagePath);
bool saveCalibration(string filename, Calibration calibration);
Calibration loadCalibration(string filename);
