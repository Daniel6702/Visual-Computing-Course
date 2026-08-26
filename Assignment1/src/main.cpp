#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

int main()
{
    cv::Mat image(100, 100, CV_8UC3, cv::Scalar(0, 0, 255));

    cv::Mat blurred;
    cv::GaussianBlur(image, blurred, cv::Size(5, 5), 0);

    std::cout << "OpenCV works!\n";
    std::cout << "Image size: "
              << blurred.cols << "x"
              << blurred.rows << '\n';

    return 0;
}