#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

inline bool setup_gl(GLFWwindow*& window, GLuint& shader, GLuint& vao, GLuint& vbo)
{
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "OpenGL Cube", nullptr, nullptr);
    if (!window) return false;

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) return false;

    const char* vertex_source = R"(
        #version 330 core

        layout(location = 0) in vec3 position;

        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * vec4(position, 1.0);
        }
    )";

    const char* fragment_source = R"(
        #version 330 core

        out vec4 color;

        void main()
        {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )";

    auto compile = [](GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        return shader;
    };

    GLuint vertex_shader = compile(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compile(GL_FRAGMENT_SHADER, fragment_source);

    shader = glCreateProgram();
    glAttachShader(shader, vertex_shader);
    glAttachShader(shader, fragment_shader);
    glLinkProgram(shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glViewport(0, 0, 800, 600);
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);

    return true;
}