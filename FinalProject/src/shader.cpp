#include "shader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

string readShader(string path) {
    ifstream file(path);
    if (!file.is_open()) {cerr << "Could not open shader: " << path << endl; return "";}
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, string source) {
    GLuint shader = glCreateShader(type);
    char const* text = source.c_str();
    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        cerr << "Shader compilation failed: " << log << endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint loadShaderProgram(string vertexPath, string fragmentPath) {
    string vertexSource = readShader(vertexPath);
    string fragmentSource = readShader(fragmentPath);
    if (vertexSource.empty() || fragmentSource.empty()) return 0;

    GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!vertex || !fragment) {
        glDeleteShader(vertex); glDeleteShader(fragment);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        cerr << "Shader linking failed: " << log << endl;
        glDeleteProgram(program);
        return 0;
    }
    return program;
}
