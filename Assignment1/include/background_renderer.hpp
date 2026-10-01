#pragma once

#include <glad/gl.h>
#include <opencv2/core.hpp>

struct BackgroundRenderer
{
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint texture = 0;
    GLuint shader = 0;
};

bool setup_background(
    BackgroundRenderer& renderer,
    const cv::Mat& first_frame,
    const char* vertex_shader_path,
    const char* fragment_shader_path
);

void draw_background(
    const BackgroundRenderer& renderer,
    const cv::Mat& frame
);

void cleanup_background(BackgroundRenderer& renderer);