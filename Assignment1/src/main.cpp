#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include <cmath>

#include "filters.hpp"
#include "transformations.hpp"
#include "projection.hpp"

using namespace cv;
using namespace std;

int CHECKPOINT = 4;


int main()
{
    if (CHECKPOINT == 3) {
        VideoCapture cap(0);
        if(!cap.isOpened()) return -1;

        Mat frame;
        namedWindow("Camera", WINDOW_AUTOSIZE);

        vector<Point3f> cube_points = get_cube_vertices();

        cube_points = translate(cube_points, -1, 0, 6);

        float t = 0.0;

        for(;;)
        {
            cap >> frame;

            t += 0.05;

            cube_points = rotate(cube_points,0.04,-0.06,0.0);

            cube_points = translate(
                cube_points,
                sin(t) * 0.05f,
                sin(t + 2 * CV_PI / 3) * 0.05f,
                sin(t + 4 * CV_PI / 3) * 0.1f
            );

            Matx33d K = get_camera_matrix(frame);

            vector<Point2f> projected = project(cube_points, K);

            int edges[][2] = {
                {0,1},{1,2},{2,3},{3,0},
                {4,5},{5,6},{6,7},{7,4},
                {0,4},{1,5},{2,6},{3,7}
            };

            for (int i = 0; i < size(edges); i++) {
                //get the indexes of the points
                int a = edges[i][0];
                int b = edges[i][1];
                //get the position of the points
                Point A(projected[a]);
                Point B(projected[b]);
                //define style
                Scalar color(0,255,0);
                int thickness = 2;
                //draw line between points
                line(frame,A,B,color,thickness);
            }

            imshow("Camera", frame);

            if(waitKey(30) >= 0) break;
        }
    } else if (CHECKPOINT == 4) {
        printf("Hello");
    }
}
