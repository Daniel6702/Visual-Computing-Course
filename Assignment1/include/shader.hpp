#pragma once

#include <glad/gl.h>

GLuint load_shader_program(
    const char* vertex_path,
    const char* fragment_path
);