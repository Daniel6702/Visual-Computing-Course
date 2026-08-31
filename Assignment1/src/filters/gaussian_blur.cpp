#include "filters/gaussian_blur.hpp"
#include "filters/kernel.hpp"

using namespace cv;

Mat gaussian_blur(Mat input) {
    float data[5][5] = { //gaussian shaped kernel
        {1,  4,  6,  4, 1},
        {4, 16, 24, 16, 4},
        {6, 24, 36, 24, 6},
        {4, 16, 24, 16, 4},
        {1,  4,  6,  4, 1}
    };

    Mat kernel2D(5, 5, CV_32F, data); //def matrix 5x5, element type 32 bit float, init with kernel

    kernel2D = normalize_kernel(kernel2D); //should sum up to 1, to not change intensity levels

    Mat output = apply_kernel2D(input, kernel2D); //apply it

    return output;
}