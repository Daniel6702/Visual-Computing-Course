#version 330 core

//inputs
layout(location = 0) in vec3 position; //position of full screen quad vertices (already normalized when defined)
layout(location = 1) in vec2 tex_coord; //position of the texture on quad

out vec2 uv;

void main()
{
    //quad already covers the screen. so the vertext position can be applied directly without projection
    gl_Position = vec4(position, 1.0);

    uv = tex_coord; //pass texture coordinates to the fragment shader
}