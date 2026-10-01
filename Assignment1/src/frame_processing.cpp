#include "frame_processing.hpp"

#include <opencv2/imgproc.hpp>

using namespace cv;

Mat process_frame(const Mat& frame, int mode) {
    if (mode == 0)
        return frame;

    Mat processed;

    //convert to gray scale
    cvtColor(frame, processed, COLOR_BGR2GRAY);

    //apply further filters based on mode.
    if (mode > 1) {
        GaussianBlur(processed,processed, Size(7, 7), 1.5, 1.5);
    }

    if (mode > 2) {
        Canny(processed, processed, 0, 30, 3);
    }

    Mat result;

    //convert back to color. for correct num of dimentions for later rendering
    cvtColor(processed, result, COLOR_GRAY2BGR);

    return result;
}