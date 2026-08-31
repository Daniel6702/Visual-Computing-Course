#include "filters/grayscale.hpp"

using namespace cv;

Mat grayscale(Mat input) {

    Mat gray(input.rows, input.cols, 0); //New empty image, same size, 0 is the type (8 bit i.e. 1 channel)

    for (int row = 0; row < input.rows; row++) {
        for (int col = 0; col < input.cols; col++) {

            //Get pixel values at each position
            Vec3b pixel = input.at<Vec3b>(row,col);

            //compute average
            int avg = (pixel[0] + pixel[1] + pixel[2]) / 3;

            //add avg to new image 
            gray.at<int8_t>(row, col) = avg;

        }
    }

    return gray;
}