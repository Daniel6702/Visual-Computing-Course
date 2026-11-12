#pragma once

#include <glad/gl.h>
#include <opencv2/core.hpp>

using namespace cv;

struct BackgroundRenderer {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint texture = 0;
    GLuint shader = 0;
};

BackgroundRenderer createBackground(Mat firstFrame);
void drawBackground(BackgroundRenderer& background, Mat frame);
void cleanupBackground(BackgroundRenderer& background);
