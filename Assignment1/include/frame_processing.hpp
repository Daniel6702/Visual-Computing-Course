#pragma once

#include <opencv2/core.hpp>

using namespace cv;

Mat process_frame(
    const cv::Mat& frame,
    int mode
);