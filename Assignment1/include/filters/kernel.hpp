#pragma once

#include "opencv2/imgproc.hpp"

cv::Mat apply_kernel2D(cv::Mat input, cv::Mat kernel2D);
cv::Mat normalize_kernel(cv::Mat kernel2D);