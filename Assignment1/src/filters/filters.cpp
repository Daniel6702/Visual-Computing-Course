#include "filters.hpp"

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

Mat show_edges(Mat input) {

    float data[3][3] = { //Sharpening / edge kernel. 2. derivative 
        {0, 1, 0},
        {1, -4, 1},
        {0, 1, 0}
    };
    Mat kernel2D(3, 3, CV_32F, data); 

    Mat gray = grayscale(input);

    Mat blur = gaussian_blur(gray);

    Mat output = apply_kernel2D(blur, kernel2D);

    return output;
}