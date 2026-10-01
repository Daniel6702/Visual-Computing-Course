#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

inline bool setup_gl(GLFWwindow*& window, GLuint& shader, GLuint& vao, GLuint& vbo)
{
    if (!glfwInit()) return false; // Initialize GLFW

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core profile

    //window = glfwCreateWindow(800, 600, "OpenGL Webcam Cube", nullptr, nullptr); // Create window

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(mode->width, mode->height, "OpenGL Webcam Cube", monitor, nullptr);

    if (!window) return false;

    glfwMakeContextCurrent(window); // Activate OpenGL context
    if (!gladLoadGL(glfwGetProcAddress)) return false; // Load OpenGL functions

    const char* vertex_source = R"(
        #version 330 core

        layout(location = 0) in vec3 position;
        layout(location = 1) in vec2 tex_coord;

        uniform mat4 view;
        uniform mat4 projection;
        uniform bool background;

        out vec2 uv;

        void main()
        {
            if (background) {
                gl_Position = vec4(position, 1.0);
                uv = tex_coord;
            }
            else {
                gl_Position = projection * view * vec4(position, 1.0);
            }
        }
    )";

    const char* fragment_source = R"(
        #version 330 core

        in vec2 uv;
        out vec4 color;

        uniform sampler2D camera_texture;
        uniform bool background;

        void main()
        {
            if (background)
                color = texture(camera_texture, uv);
            else
                color = vec4(0.0, 1.0, 0.0, 1.0);
        }
    )";

    auto compile = [](GLenum type, const char* source) {
        GLuint shader = glCreateShader(type); 
        glShaderSource(shader, 1, &source, nullptr); //Set shader source
        glCompileShader(shader); //compile shader
        return shader;
    };

    GLuint vertex_shader = compile(GL_VERTEX_SHADER, vertex_source); // Compile vertex shader
    GLuint fragment_shader = compile(GL_FRAGMENT_SHADER, fragment_source); // Compile fragment shader

    shader = glCreateProgram(); // Create shader program
    glAttachShader(shader, vertex_shader);
    glAttachShader(shader, fragment_shader);
    glLinkProgram(shader); // Link shaders

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // VAO/VBO used for the cube.
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        nullptr
    );
    glEnableVertexAttribArray(0); // Enable position attribute

    //glViewport(0, 0, 800, 600); // Set render area
    glViewport(0, 0, mode->width, mode->height);
    glClearColor(0, 0, 0, 1); // Black clear color
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    return true;
}