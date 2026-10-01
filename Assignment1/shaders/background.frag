#version 330 core

//input texture coordinates
in vec2 uv;

out vec4 color; // color written to the current screen pixel

uniform sampler2D camera_texture; //input texture 

void main()
{
    //out put is the color of the camera_texture at texture position uv
    color = texture(camera_texture, uv);
}