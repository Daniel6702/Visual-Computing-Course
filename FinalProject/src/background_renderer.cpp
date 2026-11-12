#include "background_renderer.hpp"
#include "shader.hpp"

BackgroundRenderer createBackground(Mat firstFrame) {
    BackgroundRenderer background;
    background.shader = loadShaderProgram("../shaders/background.vert", "../shaders/background.frag");
    if (!background.shader) return background;

    //Full-screen quad (two triangles), taken from the Assignment 1 renderer.
    float vertices[] = {
        -1,-1, 0,  0,1,    1,-1, 0,  1,1,    1, 1, 0,  1,0,
        -1,-1, 0,  0,1,    1, 1, 0,  1,0,   -1, 1, 0,  0,0
    };

    glGenVertexArrays(1, &background.vao);
    glGenBuffers(1, &background.vbo);
    glBindVertexArray(background.vao);
    glBindBuffer(GL_ARRAY_BUFFER, background.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &background.texture);
    glBindTexture(GL_TEXTURE_2D, background.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, firstFrame.cols, firstFrame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, firstFrame.data);

    glUseProgram(background.shader);
    glUniform1i(glGetUniformLocation(background.shader, "camera_texture"), 0);
    return background;
}

void drawBackground(BackgroundRenderer& background, Mat frame) {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(background.shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, background.texture);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)frame.step / frame.elemSize());
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glBindVertexArray(background.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void cleanupBackground(BackgroundRenderer& background) {
    glDeleteTextures(1, &background.texture);
    glDeleteBuffers(1, &background.vbo);
    glDeleteVertexArrays(1, &background.vao);
    glDeleteProgram(background.shader);
}
