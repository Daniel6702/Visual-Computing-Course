
#include <iostream>
#include <opencv2/opencv.hpp>
#include "calibrate.hpp"

using namespace cv;
using namespace std;

bool DO_CALIBRATION = true;
bool LIVE = true;
int CHECKPOINT = 2;

int main()
{
    if (CHECKPOINT == 1) {
    //https://docs.opencv.org/4.13.0/dc/dbb/tutorial_py_calibration.html

    //do or load calibration
    Calibration calibration;
    if (DO_CALIBRATION) {
        calibration = calibrate("Assignment2/imgs/*");

        saveCalibration("Assignment2/calibration.yml", calibration);

    } else {
        calibration = loadCalibration("Assignment2/calibration.yml");
    }

    VideoCapture cap(0);
    if (!cap.isOpened()) return -1;

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
            frame = imread("Assignment2/imgs/picture_2026-10-22_21-20-01.jpg");
        }
        
        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY); //convert to grayscale

        vector<Point2f> corners;
        bool found = findChessboardCorners(gray, boardSize, corners); //2d position / camera position of all inner corners

        if (found)
        {

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
but could be plausible. pretty shit camera, and it is relativly wide angle

*/













} else if (CHECKPOINT == 2) {

    //load reference image
    Mat reference = imread("Assignment2/cookbook3.jpg");

    //create SIFT detector and descriptor
    Ptr<SIFT> sift = SIFT::create();

    Mat referenceGray;
    cvtColor(reference, referenceGray, COLOR_BGR2GRAY);

    //find keypoints and compute descriptions of those points (reference)
    vector<KeyPoint> referenceKeypoints;
    Mat referenceDescriptors;
    sift->detectAndCompute(referenceGray, noArray(), referenceKeypoints, referenceDescriptors);

    // Open webcam
    VideoCapture cap(0);
    if (!cap.isOpened()) return -1;

    BFMatcher matcher(NORM_L2);

    Mat frame;
    for (;;)
    {
        cap >> frame;
        if (frame.empty()) break;

        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        //perform the same detection and description of the input frame
        vector<KeyPoint> frameKeypoints;
        Mat frameDescriptors;
        sift->detectAndCompute(gray, noArray(), frameKeypoints, frameDescriptors);

        // Match descriptors
        int k = 2;
        vector<vector<DMatch>> matches; //find the k best matches for each reference descriptor
        matcher.knnMatch(referenceDescriptors, frameDescriptors, matches, k);

        //filter away ambiguous matches
        vector<DMatch> goodMatches;
        for (vector<DMatch> match : matches) { //for each of the reference descriptor matches
            //if the best of the two matches is twice as good (less then half the distance) of the second, then we may assume it is a good not ambiguous match
            if (match[0].distance < 0.4f * match[1].distance) { 
                goodMatches.push_back(match[0]);
            }
        }

        //display correspondences
        Mat result;
        drawMatches(
            reference, 
            referenceKeypoints, 
            frame, 
            frameKeypoints, 
            goodMatches, 
            result, 
            Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
        );

        imshow("Feature Matching", result);
        if (waitKey(30) >= 0) break;
    }

}
    return 0;
}

