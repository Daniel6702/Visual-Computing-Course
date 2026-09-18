#include <iostream>
#include <cmath>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"

#include "filters.hpp"
#include "transformations.hpp"
#include "projection.hpp"
#include "opengl_boilerplate.hpp"

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
        cube_points = translate(cube_points, -1, 0, 10);

        float t = 0.0;

        for(;;)
        {
            cap >> frame;

            t += 0.05;

            cube_points = rotate(cube_points, 0.04, -0.06, 0.0);

            cube_points = translate(
                cube_points,
                sin(t) * 0.05f,
                sin(t + 2 * CV_PI / 3) * 0.05f,
                sin(t + 4 * CV_PI / 3) * 0.2f
            );

            Matx33d K = get_camera_matrix(frame);
            vector<Point2f> projected = project(cube_points, K);

            int edges[][2] = {
                {0,1},{1,2},{2,3},{3,0},
                {4,5},{5,6},{6,7},{7,4},
                {0,4},{1,5},{2,6},{3,7}
            };

            for (int i = 0; i < size(edges); i++) {
                int a = edges[i][0];
                int b = edges[i][1];

                Point A(projected[a]);
                Point B(projected[b]);

                line(frame, A, B, Scalar(0,255,0), 2);
            }

            imshow("Camera", frame);

            if(waitKey(30) >= 0) break;
        }
    }

    else if (CHECKPOINT == 4) {
        GLFWwindow* window;
        GLuint shader, vao, vbo;

        if (!setup_gl(window, shader, vao, vbo)) return -1;

        const int width = 800;
        const int height = 600;

        vector<Point3f> cube_points = get_cube_vertices();
        cube_points = translate(cube_points, -1, 0, 0);

        int edges[][2] = {
            {0,1},{1,2},{2,3},{3,0},
            {4,5},{5,6},{6,7},{7,4},
            {0,4},{1,5},{2,6},{3,7}
        };

        //camera  view is 10 units behind the scene
        Matx44f view(
            1,0,0,0,
            0,1,0,0,
            0,0,1,10,
            0,0,0,1
        );

        Matx44f projection = get_projection_matrix(width, height);

        GLint view_location = glGetUniformLocation(shader, "view");
        GLint projection_location = glGetUniformLocation(shader, "projection");

        //3d world space points are first transformed to be relative to the view /camera position
        //then the projection to 2d space
        //then rasterization -> fragment shader (color)

        float t = 0.0f;

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            t += 0.05f;

            // Same model transformations as checkpoint 3.
            cube_points = rotate(cube_points, 0.04, -0.06, 0.0);

            cube_points = translate(
                cube_points,
                sin(t) * 0.05f,
                sin(t + 2 * CV_PI / 3) * 0.05f,
                sin(t + 4 * CV_PI / 3) * 0.2f
            );

            // Convert the cube edges into 3D vertices for OpenGL.
            float vertices[72];

            for (int i = 0; i < 12; i++) {
                Point3f A = cube_points[edges[i][0]];
                Point3f B = cube_points[edges[i][1]];

                vertices[i*6 + 0] = A.x;
                vertices[i*6 + 1] = A.y;
                vertices[i*6 + 2] = A.z;

                vertices[i*6 + 3] = B.x;
                vertices[i*6 + 4] = B.y;
                vertices[i*6 + 5] = B.z;
            }

            glUseProgram(shader);

            glUniformMatrix4fv(view_location, 1, GL_TRUE, view.val);
            glUniformMatrix4fv(projection_location, 1, GL_TRUE, projection.val);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

            glDrawArrays(GL_LINES, 0, 24);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(shader);

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    return 0;
}