#include "projection.hpp"

vector<Point3f> get_cube_vertices() {
    vector<Point3f> cube = {
        {-1,-1,0},
        { 1,-1,0},
        { 1, 1,0},
        {-1, 1,0},

        {-1,-1,2},
        { 1,-1,2},
        { 1, 1,2},
        {-1, 1,2}
    };
    return cube;
}

Matx33d get_camera_matrix(Mat frame) {
    //Define camera matrix: for projection of 3D points onto 2D plane
    int width = frame.cols;
    int height = frame.rows;
    float f  = 0.9 * width;
    float cx = width * 0.5;
    float cy = height * 0.5;
    Matx33d K(f, 0, cx,
              0, f, cy,
              0, 0, 1);
    return K;
}

vector<Point2f> project(vector<Point3f> points, Matx33d K) {
    int length = points.size();
    vector<Point2f> projected(length);

    for (int i = 0; i < length; i++) {
        Vec3d P(
            points[i].x,
            points[i].y,
            points[i].z
        );

        Vec3f q = K * P;

        float x = q[0] / q[2];
        float y = q[1] / q[2];

        projected[i] = Point2f(x,y);
    }

    return projected;
}
