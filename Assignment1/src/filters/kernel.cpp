#include "filters/kernel.hpp"

using namespace cv;

/*
Add different stride support
*/

Mat normalize_kernel(Mat kernel2D) {
    int krows = kernel2D.rows;
    int kcols = kernel2D.cols;
    float sum = 0;

    for (int row = 0; row < krows; row++) {
        for (int col = 0; col < kcols; col++) {
            float kernel_val = kernel2D.at<float>(row, col);
            sum += kernel_val;
        }
    }

    return kernel2D / sum;
}

Mat __apply_kernel2D_to_channel(Mat input, Mat kernel2D) {
    int rows = input.rows;
    int cols = input.cols;

    int krows = kernel2D.rows;
    int kcols = kernel2D.cols;

    Mat result(rows, cols, 0); //type 0 -> 8bit per element -> 255 vals

    for (int row = 0; row < rows; row++) { //loop over every position in the image, with padding for kernel
        for (int col = 0; col < cols; col++) {

            float new_value = 0;

            for (int i = -krows/2; i <= krows/2; i++) { //loop over the positions of the kernel
                for (int j = -kcols/2; j <= kcols/2; j++) {

                    int input_row = row + i;
                    int input_col = col + j;

                    //skip positions outside the image -> zero padding
                    if (input_row < 0 || input_row >= rows ||
                        input_col < 0 || input_col >= cols) {
                        continue;
                    }

                    float kernel_val = kernel2D.at<float>(i + krows/2, j + kcols/2);

                    uint8_t pixel_val = input.at<uint8_t>(input_row, input_col); //uint8_t -> 8 bit -> 255 vals

                    new_value += kernel_val * pixel_val; //compute a weighted sum of the pixel and kernel values
                }
            }

            result.at<uint8_t>(row, col) = new_value; //insert the new value in the result
        }
    }

    return result;
}

Mat apply_kernel2D(Mat input, Mat kernel2D) {
    int channels = input.channels();

    if (channels == 1) {
        return __apply_kernel2D_to_channel(input, kernel2D);
    }

    else if (channels == 3) {

        std::vector<Mat> color_channels;

        split(input, color_channels); //split the image in to its color channels

        //process each channel seperately
        color_channels[0] = __apply_kernel2D_to_channel(color_channels[0], kernel2D);
        color_channels[1] = __apply_kernel2D_to_channel(color_channels[1], kernel2D);
        color_channels[2] = __apply_kernel2D_to_channel(color_channels[2], kernel2D);

        Mat result;

        merge(color_channels, result); //merge the channels again

        return result;
    }

    return Mat();
}