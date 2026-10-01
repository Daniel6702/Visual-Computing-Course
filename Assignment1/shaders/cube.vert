#version 330 core

//input vertex
layout(location = 0) in vec3 position;

uniform mat4 view; //input view matrix. (camerae position and orientation)

uniform mat4 projection; //projection matrix. 3d scene to 2d plane

void main()
{
    //transform the vertex position based on the camerate (relative to it). and project
    gl_Position = projection * view * vec4(position, 1.0);
}