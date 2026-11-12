
#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <opencv2/opencv.hpp>

#include "ar_matrices.hpp"
#include "axes_renderer.hpp"
#include "background_renderer.hpp"
#include "calibration.hpp"
#include "opengl_boilerplate.hpp"
#include "tracking.hpp"

using namespace cv;
using namespace std;

int DETECTOR_TYPE = 0; //0 = SIFT, 1 = ASIFT, 2 = ASIFT reference only

int main() {
    //Reference image must have enough distinctive details for SIFT to match.
    Mat reference = imread("../imgs/4.jpg");
    Calibration calibration = loadCalibration("../calibrations/calibration_p_05.yml");
    if (reference.empty() || calibration.cameraMatrix.empty()) {cerr << "Missing reference image or calibration.yml" << endl; return -1;}

    float boardWidth = 200.0f; //measured width of the printed image in mm (change to match your board)
    TargetTracker tracker = createTracker(reference, boardWidth, DETECTOR_TYPE);
    if (tracker.referenceDescriptors.empty()) {cerr << "No SIFT features in reference image" << endl; return -1;}

    VideoCapture cap(4, CAP_V4L2);

    cap.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CAP_PROP_FRAME_HEIGHT, 720);
    cap.set(CAP_PROP_FPS, 24);

    Mat frame;
    cap >> frame;
    std::cout << "Width (cols): " << frame.cols << "\n";
    std::cout << "Height (rows): " << frame.rows << "\n";
    std::cout << "Channels: " << frame.channels() << "\n";

    //rotate(frame, frame, ROTATE_90_COUNTERCLOCKWISE);
    if (frame.empty()) {
        cerr << "Camera must provide 640x480 (the calibration resolution)" << endl; 
        return -1;
    }

    //Undistort the camera image so OpenGL lines and the background use the same projection.
    Mat mapX, mapY;
    initUndistortRectifyMap(calibration.cameraMatrix, calibration.distCoeffs, Mat(), calibration.cameraMatrix, frame.size(), CV_16SC2, mapX, mapY);
    Matx44f projection = getProjectionMatrix(calibration.cameraMatrix, frame.size());

    GLFWwindow* window = setupGL(frame.cols/2, frame.rows/2);
    if (!window) return -1;

    BackgroundRenderer background = createBackground(frame);
    AxesRenderer axes = createAxes(tracker.width, tracker.height);
    if (!background.shader || !axes.shader) {cerr << "Could not load OpenGL shaders" << endl; return -1;}

    //Render loop. Esc closes the window; a failed detection simply hides the axes.
    while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        cap >> frame;
        //rotate(frame, frame, ROTATE_90_COUNTERCLOCKWISE);
        if (frame.empty()) break;

        Mat undistorted;
        remap(frame, undistorted, mapX, mapY, INTER_LINEAR);
        TrackingResult tracked = trackTarget(tracker, undistorted, calibration.cameraMatrix);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawBackground(background, undistorted);
        if (tracked.found) drawAxes(axes, getPoseMatrix(tracked.rvec, tracked.tvec), projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanupAxes(axes);
    cleanupBackground(background);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

