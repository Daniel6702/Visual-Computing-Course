#pragma once

#include <opencv2/core.hpp>
#include <vector>

using namespace cv;
using namespace std;

vector<Point3f> apply_transformation_matrix(
    vector<Point3f> points,
    Mat M
);

vector<Point3f> translate(
    vector<Point3f> points,
    float X,
    float Y,
    float Z
);

vector<Point3f> scale(
    vector<Point3f> points,
    float X,
    float Y,
    float Z
);

vector<Point3f> rotate(
    vector<Point3f> points,
    float X,
    float Y,
    float Z
);