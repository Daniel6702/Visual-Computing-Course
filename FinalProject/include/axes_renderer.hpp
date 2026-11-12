#pragma once

#include <glad/gl.h>
#include <opencv2/core.hpp>

using namespace cv;

struct AxesRenderer {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint shader = 0;
};

AxesRenderer createAxes(float boardWidth, float boardHeight);
void drawAxes(AxesRenderer& axes, Matx44f pose, Matx44f projection);
void cleanupAxes(AxesRenderer& axes);
