#pragma once

#include <opencv2/core.hpp>

using namespace cv;

Matx44f getProjectionMatrix(Mat cameraMatrix, Size imageSize);
Matx44f getPoseMatrix(Mat rvec, Mat tvec);
