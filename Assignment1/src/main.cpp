#include <iostream>
#include <cmath>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "transformations.hpp"
#include "projection.hpp"
#include "opengl_boilerplate.hpp"

using namespace cv;
using namespace std;

int main()
{
    // Webcam
    VideoCapture cap(0); //Open default webcam
    if (!cap.isOpened()) {
        cerr << "Could not open webcam" << endl;
        return -1;
    }

    Mat frame;
    cap >> frame;//Capture first frame

    if (frame.empty()) {
        cerr << "Could not capture webcam frame" << endl;
        return -1;
    }

    //openGL
    GLFWwindow* window;
    GLuint shader, cube_vao, cube_vbo;

    if (!setup_gl(window, shader, cube_vao, cube_vbo))//Initialize OpenGL
        return -1;

    //const int width = 800;
    //const int height = 600;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    //webcam background
    float quad_vertices[] = {
        // position       texture coordinates
        -1, -1, 0,       0, 1,
         1, -1, 0,       1, 1,
         1,  1, 0,       1, 0,

        -1, -1, 0,       0, 1,
         1,  1, 0,       1, 0,
        -1,  1, 0,       0, 0
    };

    GLuint background_vao;
    GLuint background_vbo;

    glGenVertexArrays(1, &background_vao);
    glGenBuffers(1, &background_vbo);

    glBindVertexArray(background_vao);
    glBindBuffer(GL_ARRAY_BUFFER, background_vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(quad_vertices),
        quad_vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        nullptr
    );
    glEnableVertexAttribArray(0); // Position attribute

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1); // Texture coordinate attribute

    // Webcam texture
    GLuint camera_texture;
    glGenTextures(1, &camera_texture); // Create texture
    glBindTexture(GL_TEXTURE_2D, camera_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Tightly packed pixels

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        frame.cols,
        frame.rows,
        0,
        GL_BGR,
        GL_UNSIGNED_BYTE,
        frame.data
    ); //first webcam frame

    // Cube
    vector<Point3f> cube_points = get_cube_vertices(); // Create cube vertices
    cube_points = translate(cube_points, -1, 0, 10); // Move cube forward

    int edges[][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    Matx44f view(
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    ); // Identity view matrix

    Matx44f projection = get_projection_matrix(width, height); // Perspective projection

    GLint view_location = glGetUniformLocation(shader, "view");
    GLint projection_location = glGetUniformLocation(shader, "projection");
    GLint background_location = glGetUniformLocation(shader, "background");
    GLint texture_location = glGetUniformLocation(shader, "camera_texture");

    glUseProgram(shader); //Activate shader
    glUniform1i(texture_location, 0); // Use texture unit 0

    float t = 0.0f;

    int processing_mode = 0;
    bool space_pressed = false;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        cap >> frame; //Capture current webcam frame
        if (frame.empty())
            break;

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !space_pressed) {
            processing_mode += 1;
            if (processing_mode > 3) {
                processing_mode = 0;
            } 
            space_pressed = true;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            space_pressed = false;
        }

        Mat display_frame = frame;

        if (processing_mode > 0) {
            Mat processed;

            cvtColor(frame, processed, COLOR_BGR2GRAY);

            if (processing_mode > 1)
                GaussianBlur(processed, processed, Size(7,7), 1.5, 1.5);

            if (processing_mode > 2)
                Canny(processed, processed, 0, 30, 3);

            cvtColor(processed, display_frame, COLOR_GRAY2BGR); // Convert for texture upload
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear frame

        // Webcam background
        glUseProgram(shader);
        glUniform1i(background_location, true); //Use background shader path

        glDisable(GL_DEPTH_TEST); // Background ignores depth

        glActiveTexture(GL_TEXTURE0); // Select texture unit 0
        glBindTexture(GL_TEXTURE_2D, camera_texture);

        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            display_frame.cols,
            display_frame.rows,
            GL_BGR,
            GL_UNSIGNED_BYTE,
            display_frame.data
        ); // Update webcam texture

        glBindVertexArray(background_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6); //Draw fullscreen quad

        //Update cube
        t += 0.05f;

        cube_points = rotate(cube_points, 0.04f, -0.06f, 0.0f); // Rotate cube

        cube_points = translate(
            cube_points,
            sin(t) * 0.05f,
            sin(t + 2 * CV_PI / 3) * 0.05f,
            sin(t + 4 * CV_PI / 3) * 0.2f
        ); //Move cube smoothly

        float vertices[72];

        for (int i = 0; i < 12; i++) {
            Point3f A = cube_points[edges[i][0]];
            Point3f B = cube_points[edges[i][1]];

            vertices[i * 6 + 0] = A.x;
            vertices[i * 6 + 1] = A.y;
            vertices[i * 6 + 2] = A.z;

            vertices[i * 6 + 3] = B.x;
            vertices[i * 6 + 4] = B.y;
            vertices[i * 6 + 5] = B.z;
        }

        //Draw cube partr
        glEnable(GL_DEPTH_TEST); //enalbe depth testing again

        glUniform1i(background_location, false); //not the backgrnd cube
        glUniformMatrix4fv(view_location, 1, GL_TRUE, view.val); //Upload view matrix
        glUniformMatrix4fv(projection_location, 1, GL_TRUE, projection.val); //Upload projection

        glBindVertexArray(cube_vao);

        glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_DYNAMIC_DRAW
        ); // Upload updated cube

        glDrawArrays(GL_LINES, 0, 24); //Draw 12 cube edges

        glfwSwapBuffers(window);//Display completed frame
        glfwPollEvents(); // Process window events
    }

    // Cleanup
    glDeleteTextures(1, &camera_texture);

    glDeleteBuffers(1, &background_vbo);
    glDeleteVertexArrays(1, &background_vao);

    glDeleteBuffers(1, &cube_vbo);
    glDeleteVertexArrays(1, &cube_vao);

    glDeleteProgram(shader);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}