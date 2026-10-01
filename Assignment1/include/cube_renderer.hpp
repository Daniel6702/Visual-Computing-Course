#pragma once

#include <glad/gl.h>
#include <opencv2/core.hpp>

#include <vector>

using namespace std;
using namespace cv;

struct CubeRenderer
{
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint shader = 0;

    vector<cv::Point3f> points;

    float time = 0.0f;
};

bool setup_cube(
    CubeRenderer& cube,
    const char* vertex_shader_path,
    const char* fragment_shader_path
);

void update_cube(CubeRenderer& cube);

void draw_cube(
    const CubeRenderer& cube,
    const Matx44f& view,
    const Matx44f& projection
);

void cleanup_cube(CubeRenderer& cube);