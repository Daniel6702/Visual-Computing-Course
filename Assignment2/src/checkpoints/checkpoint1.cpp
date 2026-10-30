#include <iostream>
#include <opencv2/opencv.hpp>
#include "calibrate.hpp"
#include "checkpoints.hpp"

using namespace cv;
using namespace std;

int checkpoint1(bool DO_CALIBRATION, bool LIVE) {

    //do or load calibration
    Calibration calibration;
    if (DO_CALIBRATION) {
        calibration = calibrate("../imgs/calibration/*");

        saveCalibration("../calibration.yml", calibration);

    } else {
        calibration = loadCalibration("../calibration.yml");
    }

    VideoCapture cap(0);
    //if (!cap.isOpened()) return -1;

    Size boardSize(7, 6);
    float squareSize = 25.0f; //25 mm (i.e. coordinates are in mm)

    //checkerboard coordinates in mm
    vector<Point3f> objp;
    for (int y = 0; y < boardSize.height; y++) {
        for (int x = 0; x < boardSize.width; x++) {
            objp.emplace_back(x * squareSize, y * squareSize, 0);
        }
    }

    Mat frame;

    for (;;)
    {
        if (LIVE) {
            cap >> frame; //capture frame
        } else {
            frame = imread("../imgs/calibration/picture_2026-10-22_21-20-01.jpg");
        }
        
        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY); //convert to grayscale

        vector<Point2f> corners;
        bool found = findChessboardCorners(gray, boardSize, corners); //2d position / camera position of all inner corners

        if (found) {

            //estimate checkerboard pose. i.e. translation and rotation relative to camera
            //based on detected corner position and known relative positions of corners.
            //and our calibrated camera matrix and distortion
            Mat rvec, tvec;
            bool success = solvePnP( 
                objp, 
                corners, 
                calibration.cameraMatrix, 
                calibration.distCoeffs, 
                rvec, 
                tvec
            );

            if (success)
            {
                // Project coordinate axes onto the image
                vector<Point2f> projected;
                vector<Point3f> axes = { //define the four relative points for the coordinate  system
                    {0, 0, 0}, 
                    {75, 0, 0}, //each line is 75 mm
                    {0, 75, 0}, 
                    {0, 0, -75}
                };
                //translate and rotate those points according to the checkerboard,
                //then project to 2D, with our calibrated matrix and distortion
                projectPoints(axes, rvec, tvec, calibration.cameraMatrix, calibration.distCoeffs, projected);

                //draw the 2d lines
                line(frame, projected[0], projected[1], Scalar(0, 0, 255), 3);
                line(frame, projected[0], projected[2], Scalar(0, 255, 0), 3);
                line(frame, projected[0], projected[3], Scalar(255, 0, 0), 3);
            }
        }

        imshow("Camera", frame);
        if (waitKey(30) >= 0) break;
    }

    return 0;

/*
evaluate whether the calibration result is reasonable:
Camera matrix:
[541.4390609586059, 0, 318.1962389388054;
 0, 542.1881403860657, 236.0730057915562;
 0, 0, 1]

Focal lengths are about equal, which makes sense since square pixels
Principal point also is very close to center given resultion of 640×480
Skew is zero which is expected. i.e. sensor lens allignment

Distortion coefficients:
[-0.1605262553847396, 1.038727910210042, 0.0003420796410913305, -0.009595532870601404, -2.13541895084304]
Reprojection RMS error: 0.613784 pixels

k2 and k3 are relativly high. these handle radial distortion (Barrel, Pincushion Distortion)
but could be plausible. pretty shit camera/lens, and it is relativly wide angle

*/

}