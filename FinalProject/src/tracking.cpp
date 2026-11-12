#include "tracking.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

TargetTracker createTracker(Mat reference, float width, int detectorType) {
    TargetTracker tracker;
    tracker.width = width;
    tracker.height = width * reference.rows / reference.cols; //keep the printed image's proportions
    tracker.referenceSize = reference.size();
    Ptr<Feature2D> sift = SIFT::create();
    Ptr<Feature2D> asift = AffineFeature::create(sift);
    tracker.referenceDetector = detectorType == 0 ? sift : asift;
    tracker.frameDetector = detectorType == 1 ? asift : sift;

    Mat gray;
    cvtColor(reference, gray, COLOR_BGR2GRAY);
    tracker.referenceDetector->detectAndCompute(gray, noArray(), tracker.referenceKeypoints, tracker.referenceDescriptors);
    return tracker;
}

TrackingResult estimateTargetPose(TargetTracker& tracker, Mat frame, Mat cameraMatrix) {
    TrackingResult result;

    Mat gray;
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    //detect SIFT features in the current webcam frame
    vector<KeyPoint> frameKeypoints;
    Mat frameDescriptors;
    tracker.frameDetector->detectAndCompute(gray, noArray(), frameKeypoints, frameDescriptors);
    if (frameDescriptors.rows < 2) return result;

    //match descriptors and reject ambiguous matches using the ratio test
    BFMatcher matcher(NORM_L2);
    vector<vector<DMatch>> matches;
    matcher.knnMatch(tracker.referenceDescriptors, frameDescriptors, matches, 2);

    vector<Point2f> referencePoints, framePoints;
    for (vector<DMatch>& match : matches) {
        if (match.size() < 2 || match[0].distance >= 0.75f * match[1].distance) continue;
        referencePoints.push_back(tracker.referenceKeypoints[match[0].queryIdx].pt);
        framePoints.push_back(frameKeypoints[match[0].trainIdx].pt);
    }
    if (referencePoints.size() < 8) return result;

    //estimate the transformation between the reference and the current image
    //RANSAC rejects inconsistent matches, just as in Assignment 2
    Mat inlierMask;
    Mat homography = findHomography(referencePoints, framePoints, RANSAC, 3.0, inlierMask);
    if (homography.empty() || countNonZero(inlierMask) < 8) return result;

    vector<Point2f> referenceCorners = {
        {0, 0}, {(float)tracker.referenceSize.width, 0},
        {(float)tracker.referenceSize.width, (float)tracker.referenceSize.height},
        {0, (float)tracker.referenceSize.height}
    };
    perspectiveTransform(referenceCorners, result.corners, homography);

    //convert the accepted reference-image points to physical positions on the board
    //using all inlier correspondences gives solvePnP more measurements than just 4 corners
    vector<Point3f> objectPoints;
    vector<Point2f> imagePoints;
    for (int i = 0; i < referencePoints.size(); i++) {
        if (!inlierMask.at<uchar>(i)) continue;
        float x = referencePoints[i].x * tracker.width / tracker.referenceSize.width;
        float y = referencePoints[i].y * tracker.height / tracker.referenceSize.height;
        objectPoints.push_back(Point3f(x, y, 0));
        imagePoints.push_back(framePoints[i]);
    }

    //the input frame is already undistorted, so no distortion coefficients are needed here
    result.found = solvePnP(objectPoints, imagePoints, cameraMatrix, Mat(), result.rvec, result.tvec);
    result.inliers = (int)objectPoints.size();
    if (result.found && result.tvec.at<double>(2) <= 0) result.found = false;
    return result;
}

//On failed detections, continue rendering the camera background without the axes.
TrackingResult trackTarget(TargetTracker& tracker, Mat frame, Mat cameraMatrix) {
    try {
        return estimateTargetPose(tracker, frame, cameraMatrix);
    } catch (Exception& error) {
        static bool reported = false;
        if (!reported) {cerr << "Tracking error: " << error.what() << endl; reported = true;}
        return TrackingResult();
    }
}
