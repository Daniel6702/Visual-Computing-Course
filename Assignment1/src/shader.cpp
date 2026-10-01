#include "shader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;


static string read_file(const char* path) {
    //for reading the shader filesss
    ifstream file(path);

    if (!file.is_open()) {
        cerr << "Could not open shader file: " << path << endl;
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

static GLuint compile_shader(GLenum type, const string& source) {
    //define shader instance with type
    GLuint shader = glCreateShader(type);

    const char* source_ptr = source.c_str();
    
    //compile the shader code from the source file
    glShaderSource(shader, 1, &source_ptr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {cout << ":( compile_shader";}

    return shader;
}

GLuint load_shader_program(const char* vertex_path, const char* fragment_path) {
    string vertex_source = read_file(vertex_path);
    string fragment_source = read_file(fragment_path);

    if (vertex_source.empty() || fragment_source.empty())
        return 0;

    //compile each shader
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);

    //link them together in a shader program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {cout << ":( load_shader_program";}

    //delete the seperate compiled shaders from mem
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}