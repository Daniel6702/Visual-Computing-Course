
#include "calibrate.hpp"

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Calibration calibrate(string imagePath)
{
    //define the board
    Size boardSize(7, 6);
    float squareSize = 25.0f; //25 mm per square

    //compute board 2d relative coordinates
    vector<Point3f> objp;
    for (int y = 0; y < boardSize.height; y++) {
        for (int x = 0; x < boardSize.width; x++) {
            objp.emplace_back(x * squareSize, y * squareSize, 0);
        }
    }

    vector<vector<Point3f>> objpoints;
    vector<vector<Point2f>> imgpoints;

    vector<String> files;
    glob(imagePath, files);

    Size imageSize;

    for (string file : files)
    {
        //load image
        Mat img = imread(file);
        if (img.empty()) continue;
        imageSize = img.size();

        //convert to gray
        Mat gray;
        cvtColor(img, gray, COLOR_BGR2GRAY);

        vector<Point2f> corners;
        bool found = findChessboardCorners(gray, boardSize, corners); //attempt to detect inner all innr corners

        if (found) {
            //for each image we get a list of detected coordinates and real
            objpoints.push_back(objp);
            imgpoints.push_back(corners);
        }

        cout << file << ": " << (found ? "Found" : "Not found") << endl;
    }

    Calibration result; //a struct that contains the calibrated matrix and distortion
    vector<Mat> rvecs, tvecs;

    double error = calibrateCamera(
        objpoints, 
        imgpoints, 
        imageSize, 
        result.cameraMatrix, 
        result.distCoeffs, 
        rvecs, tvecs
    );

    cout << "\nCalibration images: " << imgpoints.size() << endl;
    cout << "Camera matrix:\n" << result.cameraMatrix << endl;
    cout << "Distortion coefficients:\n" << result.distCoeffs << endl;
    cout << "Reprojection RMS error: " << error << " pixels" << endl;

    return result;
}

bool saveCalibration(string filename, Calibration calibration) {
    FileStorage fs(filename, FileStorage::WRITE);

    fs << "camera_matrix" << calibration.cameraMatrix;
    fs << "distortion_coefficients" << calibration.distCoeffs;
    fs.release();

    return true;
}

Calibration loadCalibration(string filename) {
    FileStorage fs(filename, FileStorage::READ);

    Calibration result;
    fs["camera_matrix"] >> result.cameraMatrix;
    fs["distortion_coefficients"] >> result.distCoeffs;
    fs.release();

    cout << "Camera matrix:\n" << result.cameraMatrix << endl;
    cout << "Distortion coefficients:\n" << result.distCoeffs << endl;

    return result;
}
