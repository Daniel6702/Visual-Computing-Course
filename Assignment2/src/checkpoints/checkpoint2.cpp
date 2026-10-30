#include <iostream>
#include <opencv2/opencv.hpp>
#include "calibrate.hpp"
#include "checkpoints.hpp"

using namespace cv;
using namespace std;

int checkpoint2() {

    //load reference image
    Mat reference = imread("../imgs/target.png");

    //create SIFT detector and descriptor
    Ptr<SIFT> sift = SIFT::create();

    Mat referenceGray;
    cvtColor(reference, referenceGray, COLOR_BGR2GRAY);

    //find keypoints and compute descriptions of those points (reference)
    vector<KeyPoint> referenceKeypoints;
    Mat referenceDescriptors;
    sift->detectAndCompute(referenceGray, noArray(), referenceKeypoints, referenceDescriptors);

    //open webcam
    VideoCapture cap(0);

    //init matches
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

        //match descriptors. brute force approach, find distance to all, select top k
        int k = 2;
        vector<vector<DMatch>> matches; //find the k best matches for each reference descriptor
        matcher.knnMatch(referenceDescriptors, frameDescriptors, matches, k);

        //filter away ambiguous matches
        vector<DMatch> goodMatches;
        for (vector<DMatch> match : matches) { //for each of the reference descriptor matches
            //if the best of the two matches is twice as good (less then half the distance) of the second, then we may assume it is a good not ambiguous match
            if (match[0].distance < 0.5f * match[1].distance) { 
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
    return 0;
}