#pragma once

#include <opencv2/core.hpp>
#include <opencv2/features.hpp>
#include <vector>

using namespace cv;
using namespace std;

struct TargetTracker {
    Ptr<Feature2D> referenceDetector;
    Ptr<Feature2D> frameDetector;
    vector<KeyPoint> referenceKeypoints;
    Mat referenceDescriptors;
    Size referenceSize;
    float width = 0;  //physical dimensions in millimetres
    float height = 0;
};

struct TrackingResult {
    bool found = false;
    Mat rvec;
    Mat tvec;
    vector<Point2f> corners;
    int inliers = 0;
};

TargetTracker createTracker(Mat reference, float width, int detectorType);
TrackingResult trackTarget(TargetTracker& tracker, Mat frame, Mat cameraMatrix);
