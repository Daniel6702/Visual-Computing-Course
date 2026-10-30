
#include <iostream>
#include <opencv2/opencv.hpp>
#include "calibrate.hpp"
#include "checkpoints.hpp"

using namespace cv;
using namespace std;

//SETTINGS
bool DO_CALIBRATION = false;
bool LIVE = false;
int CHECKPOINT = 3;

int main() {
    if (CHECKPOINT == 1) {
        return checkpoint1(DO_CALIBRATION, LIVE);
    } else if (CHECKPOINT == 2) {
        return checkpoint2();
    } else if (CHECKPOINT == 3) {
        return checkpoint3();
    }
    return 0;
}

