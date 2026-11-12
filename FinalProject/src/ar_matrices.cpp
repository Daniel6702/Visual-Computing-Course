#include "ar_matrices.hpp"

#include <opencv2/calib3d.hpp>

Matx44f getProjectionMatrix(Mat cameraMatrix, Size imageSize) {
    //Projection from calibrated camera intrinsics, rather than an approximate focal length.
    //The camera background is undistorted, so it follows the same pinhole projection.
    float fx = cameraMatrix.at<double>(0, 0);
    float fy = cameraMatrix.at<double>(1, 1);
    float skew = cameraMatrix.at<double>(0, 1);
    float cx = cameraMatrix.at<double>(0, 2);
    float cy = cameraMatrix.at<double>(1, 2);
    float w = imageSize.width, h = imageSize.height;
    float near = 1.0f, far = 5000.0f; //millimetres, same units as the printed target

    return Matx44f(
         2*fx/w, -2*skew/w, 1-2*cx/w,                     0,
         0,       2*fy/h,   2*cy/h-1,                     0,
         0,       0,       -(far+near)/(far-near), -2*far*near/(far-near),
         0,       0,       -1,                            0
    );
}

Matx44f getPoseMatrix(Mat rvec, Mat tvec) {
    //OpenCV camera: X right, Y down, Z forward.
    //OpenGL camera: X right, Y up, Z backwards. Flip Y and Z camera axes.
    Mat rotation;
    Rodrigues(rvec, rotation);

    Matx44f pose = Matx44f::eye();
    for (int row = 0; row < 3; row++) {
        float sign = row == 0 ? 1.0f : -1.0f;
        for (int col = 0; col < 3; col++) pose(row, col) = sign * rotation.at<double>(row, col);
        pose(row, 3) = sign * tvec.at<double>(row);
    }
    return pose;
}
