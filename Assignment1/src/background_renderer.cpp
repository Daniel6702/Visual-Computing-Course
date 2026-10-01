#include "background_renderer.hpp"
#include "shader.hpp"

bool setup_background(
    BackgroundRenderer& renderer,
    const cv::Mat& first_frame,
    const char* vertex_shader_path,
    const char* fragment_shader_path
)
{
    //load background shader
    renderer.shader = load_shader_program(
        vertex_shader_path,
        fragment_shader_path
    );

    if (!renderer.shader)
        return false;

    //full-screen quad made from two triangles
    float vertices[] = {
        //position        //texture coordinates
        -1, -1, 0,        0, 1,
         1, -1, 0,        1, 1,
         1,  1, 0,        1, 0,

        -1, -1, 0,        0, 1,
         1,  1, 0,        1, 0,
        -1,  1, 0,        0, 0
    };

    //create quad VAO and VBO
    glGenVertexArrays(1, &renderer.vao);
    glGenBuffers(1, &renderer.vbo);

    glBindVertexArray(renderer.vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //upload vertices.
    //static draw since backjground does not move

    //position attribute. vao telling how read the position (i.e. 3 floats, every 5 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);

    glEnableVertexAttribArray(0);

    //texture coordinate attribute. 2 floats every 5 floats, skip 3
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(1);

    //create webcam texture
    glGenTextures(1, &renderer.texture);

    glBindTexture(
        GL_TEXTURE_2D,
        renderer.texture
    );

    //texture filtering. make it fit the screen
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //prevent texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //upload first webcam frame
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        first_frame.cols,
        first_frame.rows,
        0,
        GL_BGR,
        GL_UNSIGNED_BYTE,
        first_frame.data
    );

    glUseProgram(renderer.shader);

    //connect shader sampler to texture unit 0
    GLint texture_location =
        glGetUniformLocation(
            renderer.shader,
            "camera_texture"
        );

    glUniform1i(texture_location, 0);

    return true;
}

void draw_background(const BackgroundRenderer& renderer, const cv::Mat& frame) {
    
    glDisable(GL_DEPTH_TEST); //draw background without depth testing

    glUseProgram(renderer.shader);

    glBindTexture(GL_TEXTURE_2D, renderer.texture);

    //update texture with current webcam frame
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        frame.cols,
        frame.rows,
        GL_BGR,
        GL_UNSIGNED_BYTE,
        frame.data
    );

    glBindVertexArray(renderer.vao);

    glDrawArrays(GL_TRIANGLES, 0, 6); //draw the two triangles
}

void cleanup_background(BackgroundRenderer& renderer)
{
    //delete OpenGL resources
    glDeleteTextures(1, &renderer.texture);
    glDeleteBuffers(1, &renderer.vbo);
    glDeleteVertexArrays(1, &renderer.vao);
    glDeleteProgram(renderer.shader);
}