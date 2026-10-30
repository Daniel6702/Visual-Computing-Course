
#include <iostream>
#include <opencv2/opencv.hpp>
#include "calibrate.hpp"
#include "checkpoints.hpp"

using namespace cv;
using namespace std;

int checkpoint3() {

    //load calibration
    Calibration calibration = loadCalibration("../calibration.yml");
    //load target image
    Mat reference = imread("../imgs/cookbook3.jpg");

    //physical target dimensions in mm
    float targetWidth = 190.0f; //210
    float targetHeight = 250.0f; //210

    vector<Point3f> objectCorners = {
        {0, 0, 0}, 
        {targetWidth, 0, 0},
        {targetWidth, targetHeight, 0},
        {0, targetHeight, 0}
    };  

    vector<Point2f> referenceCorners = {
        {0, 0},
        {reference.cols, 0},
        {reference.cols, reference.rows},
        {0, reference.rows}
    };

    //create SIFT detector and descriptor
    Ptr<SIFT> sift = SIFT::create();

    //convert reference / target image to grayscale
    Mat referenceGray;
    cvtColor(reference, referenceGray, COLOR_BGR2GRAY);

    //find keypoints and compute descriptions of those points (reference)
    vector<KeyPoint> referenceKeypoints;
    Mat referenceDescriptors;
    sift->detectAndCompute(referenceGray, noArray(), referenceKeypoints, referenceDescriptors);

    //open webcam
    VideoCapture cap(0);

    //init matcher
    BFMatcher matcher(NORM_L2);

    Mat frame;
    for (;;) {
        //capture webcame frame
        cap >> frame;

        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY); //convert to grayscale

        //use sift to detect and compute describtions from the webcam frame
        vector<KeyPoint> frameKeypoints;
        Mat frameDescriptors;
        sift->detectAndCompute(gray, noArray(), frameKeypoints, frameDescriptors);

        //we need atleast two descriptions from the frme
        if (!frameDescriptors.empty() && frameDescriptors.rows >= 2)
        {
            //match descriptors between reference and webcame frame
            //brute force approach, find distance to all, select top k
            int k = 2;
            vector<vector<DMatch>> matches;
            matcher.knnMatch(referenceDescriptors, frameDescriptors, matches, k);

            vector<Point2f> referencePoints, framePoints;

            //filter away ambiguous matches
            for (vector<DMatch> match : matches) {
                //if the best of the two matches is twice as good (less then half the distance) of the second, then we may assume it is a good not ambiguous match
                if (match[0].distance < 0.5f * match[1].distance) { 
                    //save the 2d location of that descriptor in both the reference image and the frame image. correct?
                    referencePoints.push_back(
                        referenceKeypoints[match[0].queryIdx].pt
                    );
                    framePoints.push_back(
                        frameKeypoints[match[0].trainIdx].pt
                    );
                    //they keep the same ordering. each index match the same descriptor
                }
            }

            //we need at least four corresponding point pairs to estimate a homography.
            if (referencePoints.size() >= 4) {


                // Estimate homography and reject outliers using RANSAC
                Mat inliers; //mask of accepted matches
                //the referencePoints and framePoints correspond to the same positions on the same object
                //based on their differences we can determine the transformation (homography) required to get those positions in the frame object
                Mat homography = findHomography(referencePoints, framePoints, RANSAC, 3.0, inliers);
                //RANSAC rejects the matches that are not consitent with estimated homography

                if (!homography.empty() && countNonZero(inliers) >= 4)
                {
                    // Project reference corners into camera image
                    vector<Point2f> imageCorners;
                    //referenceCorners is the approximate position of the four corners of the object in the reference image
                    //those are transformed based on the homography to estimate the relative corner positions in the frame
                    perspectiveTransform(referenceCorners, imageCorners, homography);

                    //estimate target rotation and translation
                    //i.e. how must the cookbook be rotated and translated in 3D for its four known corners to appear at these four pixel positions
                    //while taking the camera distortion into account
                    Mat rvec, tvec;
                    bool success = solvePnP(objectCorners, imageCorners, calibration.cameraMatrix, calibration.distCoeffs, rvec, tvec);
                    //now we esssentially have an estimate of the 3d position/rotation in world space of the book

                    if (success) {
  
                        vector<Point3f> axes = { ////define the four relative points for the coordinate  system
                            {0, 0, 0},
                            {75, 0, 0}, //75 mm
                            {0, 75, 0},
                            {0, 0, -75}
                        };

                        vector<Point2f> projected;
                        // translate and rotate those points according to our estiamted translation and rotation of the book
                        projectPoints(axes, rvec, tvec, calibration.cameraMatrix, calibration.distCoeffs, projected);

                        //draw the 2d lines based on the projected 2d points
                        line(frame, projected[0], projected[1], Scalar(0, 0, 255), 3);
                        line(frame, projected[0], projected[2], Scalar(0, 255, 0), 3);
                        line(frame, projected[0], projected[3], Scalar(255, 0, 0), 3);
                    }

                    //draw detected target boundary
                    for (int i = 0; i < 4; i++) {
                        line(frame, 
                            imageCorners[i], 
                            imageCorners[(i + 1) % 4], 
                            Scalar(255, 255, 0), 2);
                    }
                }
            }
        }

        imshow("Target Pose Tracking", frame);
        if (waitKey(30) >= 0) break;
    }

    return 0;
}
