#include "opengl_boilerplate.hpp"

GLFWwindow* setupGL(int width, int height) {
    if (!glfwInit()) return nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width * 2, height * 2, "AR Labyrinth - Target Tracking", nullptr, nullptr);
    if (!window) {glfwTerminate(); return nullptr;}

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    //Maintain the camera's aspect ratio if the user resizes the window.
    glfwSetWindowAspectRatio(window, width, height);
    glfwSwapInterval(1);
    glClearColor(0, 0, 0, 1);
    return window;
}
