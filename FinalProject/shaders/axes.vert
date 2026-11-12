#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColor;
out vec3 lineColor;

uniform mat4 pose;       //physical target to OpenGL camera coordinates
uniform mat4 projection; //calibrated camera projection

void main() {
    gl_Position = projection * pose * vec4(position, 1.0);
    lineColor = vertexColor;
}
