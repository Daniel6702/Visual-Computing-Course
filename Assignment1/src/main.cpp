#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include "background_renderer.hpp"
#include "cube_renderer.hpp"
#include "frame_processing.hpp"
#include "opengl_boilerplate.hpp"
#include "projection.hpp"

using namespace cv;
using namespace std;

int main()
{
    // Webcam
    VideoCapture cap(0);
    Mat frame;
    cap >> frame;

    // OpenGL
    GLFWwindow* window = nullptr;

    if (!setup_gl(window))
        return -1;

    int width;
    int height;

    glfwGetFramebufferSize(window, &width, &height);

    //background
    BackgroundRenderer background;
    setup_background(background, frame,
        "shaders/background.vert",
        "shaders/background.frag"
    );

    // Cube
    CubeRenderer cube;
    setup_cube(cube,
        "shaders/cube.vert",
        "shaders/cube.frag"
    );

    //view camera matrix
    Matx44f view(
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    );

    Matx44f projection = get_projection_matrix(width, height); //projection matrix k

    int processing_mode = 0; //filtering state
    bool space_pressed = false;

    // Main loop
    while (!glfwWindowShouldClose(window)) {

        cap >> frame; //capture webcam frame

        if (frame.empty()) {break;}

        //cycle through filters when pressen space
        //if space is pressed down
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !space_pressed) {
            processing_mode += 1;
            if (processing_mode > 3) {processing_mode=0;}
            space_pressed = true;
        }
        //if space releaseed
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            space_pressed = false;
        }

        //apply filter to webcam image based on current mode
        Mat display_frame = process_frame(frame, processing_mode);

        //Render
        //clear the background (black) and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_background(background, display_frame);

        update_cube(cube);

        draw_cube(cube, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //cleanup
    cleanup_background(background);
    cleanup_cube(cube);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}