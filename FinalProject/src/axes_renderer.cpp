#include "axes_renderer.hpp"
#include "shader.hpp"

#include <algorithm>
#include <vector>

using namespace std;

AxesRenderer createAxes(float boardWidth, float boardHeight) {
    AxesRenderer axes;
    axes.shader = loadShaderProgram("../shaders/axes.vert", "../shaders/axes.frag");
    if (!axes.shader) return axes;

    float length = min(boardWidth, boardHeight) * 0.35f;
    //Each vertex: XYZ position (mm) and RGB color.
    //Reference image origin is its top-left corner: X right, Y down, -Z out of the board.
    vector<float> vertices = {
        0,0,0,  1,0,0,    length,0,0,  1,0,0,       //X red
        0,0,0,  0,1,0,    0,length,0,  0,1,0,       //Y green
        0,0,0,  0.2f,0.5f,1,    0,0,-length,  0.2f,0.5f,1, //Z blue

        //Outline of the tracked image, to visualize alignment with the physical target.
        0,0,0,                 1,1,1,    boardWidth,0,0,           1,1,1,
        boardWidth,0,0,        1,1,1,    boardWidth,boardHeight,0, 1,1,1,
        boardWidth,boardHeight,0,1,1,1, 0,boardHeight,0,          1,1,1,
        0,boardHeight,0,       1,1,1,    0,0,0,                    1,1,1
    };

    glGenVertexArrays(1, &axes.vao);
    glGenBuffers(1, &axes.vbo);
    glBindVertexArray(axes.vao);
    glBindBuffer(GL_ARRAY_BUFFER, axes.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    return axes;
}

void drawAxes(AxesRenderer& axes, Matx44f pose, Matx44f projection) {
    glEnable(GL_DEPTH_TEST);
    glUseProgram(axes.shader);
    //Matx44f is row-major; OpenGL wants column-major, so transpose on upload.
    glUniformMatrix4fv(glGetUniformLocation(axes.shader, "pose"), 1, GL_TRUE, pose.val);
    glUniformMatrix4fv(glGetUniformLocation(axes.shader, "projection"), 1, GL_TRUE, projection.val);
    glBindVertexArray(axes.vao);
    glDrawArrays(GL_LINES, 0, 14);
}

void cleanupAxes(AxesRenderer& axes) {
    glDeleteBuffers(1, &axes.vbo);
    glDeleteVertexArrays(1, &axes.vao);
    glDeleteProgram(axes.shader);
}
