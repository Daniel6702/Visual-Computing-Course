#include "opengl_boilerplate.hpp"

bool setup_gl(GLFWwindow*& window) {
    //initiate functions
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //get current monitor for window size
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    //create window
    window = glfwCreateWindow(
        mode->width,
        mode->height,
        "OpenGL Webcam Cube",
        monitor,
        nullptr
    );

    if (!window)
        return false;

    //make the window the context. future opengl command applies to this window
    glfwMakeContextCurrent(window);

    //points glad to open gl functions in mem
    if (!gladLoadGL(glfwGetProcAddress))
        return false;

    glViewport(0, 0, mode->width, mode->height); //defines the rendering window
    glClearColor(0, 0, 0, 1); //sets the background to blacl
    glEnable(GL_DEPTH_TEST); //enables the depth buffer

    return true;
}