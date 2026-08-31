#include "filters/edges.hpp"
#include "filters/kernel.hpp"
#include "filters/grayscale.hpp"
#include "filters/gaussian_blur.hpp"


using namespace cv;

Mat show_edges(Mat input) {
    Mat kernel2D = (cv::Mat_<float>(3, 3) <<
        0, 1, 0,
        1, -4, 1,
        0, 1, 0
    );

    Mat gray = grayscale(input);

    Mat blur = gaussian_blur(gray);

    Mat output = apply_kernel2D(blur, kernel2D);

    return output;
}