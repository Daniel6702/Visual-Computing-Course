#pragma once

#include <opencv2/imgproc.hpp>

cv::Mat grayscale(cv::Mat input);

cv::Mat gaussian_blur(cv::Mat input);

cv::Mat show_edges(cv::Mat input);

cv::Mat apply_kernel2D(cv::Mat input, cv::Mat kernel2D);

cv::Mat normalize_kernel(cv::Mat kernel2D);