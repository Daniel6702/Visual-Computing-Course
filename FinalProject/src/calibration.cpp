#include "calibration.hpp"

#include <opencv2/core/persistence.hpp>

Calibration loadCalibration(string filename) {
    FileStorage file(filename, FileStorage::READ);
    Calibration calibration;

    file["camera_matrix"] >> calibration.cameraMatrix;
    file["distortion_coefficients"] >> calibration.distCoeffs;

    return calibration;
}
