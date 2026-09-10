#include "transformations.hpp"

Point3f _find_center(vector<Point3f> points) {
    Point3f center(0, 0, 0);

    for (int i = 0; i < points.size(); i++) {
        center += points[i];
    }

    center *= 1.0f / points.size();

    return center;
}

vector<Point3f> apply_transformation_matrix(vector<Point3f> points, Mat M) {
    for (int i = 0; i < points.size(); i++) {
        //cartisian to homogeneous coordinate
        Vec4f point(
            points[i].x,
            points[i].y,
            points[i].z,
            1.0f
        );

        //apply translation matrix to point
        Mat result = M * Mat(point);

        //update the point
        points[i].x = result.at<float>(0);
        points[i].y = result.at<float>(1);
        points[i].z = result.at<float>(2);
    }
    return points;
}

vector<Point3f> translate(vector<Point3f> points, float X, float Y, float Z) {
    float Tf[4][4] = {
        {1, 0, 0, X},
        {0, 1, 0, Y},
        {0, 0, 1, Z},
        {0, 0, 0, 1}
    };
    Mat T(4, 4, CV_32F, Tf); 

    points = apply_transformation_matrix(points, T);
    
    return points;
}

vector<Point3f> scale(vector<Point3f> points, float X, float Y, float Z) {
    float Sf[4][4] = {
        {X, 0, 0, 0},
        {0, Y, 0, 0},
        {0, 0, Z, 0},
        {0, 0, 0, 1}
    };
    Mat S(4, 4, CV_32F, Sf); 

    points = apply_transformation_matrix(points, S);
    
    return points;
}

vector<Point3f> rotate(vector<Point3f> points, float X, float Y, float Z) {

    Point3f center = _find_center(points);

    //move points (center) to origin. 
    points = translate(points, -center.x, -center.y, -center.z);

    float RXf[4][4] = {
        {1, 0,      0,       0},
        {0, cos(X), -sin(X), 0},
        {0, sin(X), cos(X),  0},
        {0, 0,      0,       1}
    };
    Mat RX(4, 4, CV_32F, RXf); 

    float RYf[4][4] = {
        {cos(Y),  0, sin(Y), 0},
        {0,       1, 0,      0},
        {-sin(Y), 0, cos(Y), 0},
        {0,       0, 0,      1}
    };
    Mat RY(4, 4, CV_32F, RYf); 

    float RZf[4][4] = {
        {cos(Z), -sin(Z),0, 0},
        {sin(Z), cos(Z), 0, 0},
        {0,      0,      1, 0},
        {0,      0,      0, 1}
    };
    Mat RZ(4, 4, CV_32F, RZf); 

    points = apply_transformation_matrix(points,RX);
    points = apply_transformation_matrix(points,RY);
    points = apply_transformation_matrix(points,RZ);

    //move points back
    points = translate(points, center.x, center.y, center.z);
    
    return points;
}