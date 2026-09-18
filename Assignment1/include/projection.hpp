#pragma once

#include <opencv2/core.hpp>
#include <vector>

using namespace cv;
using namespace std;

Matx33d get_camera_matrix(Mat frame);

vector<Point2f> project(vector<Point3f> points, Matx33d K);

vector<Point3f> get_cube_vertices();

Matx44f get_projection_matrix(int width, int height);