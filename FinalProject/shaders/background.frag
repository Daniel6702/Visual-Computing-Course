#version 330 core

in vec2 uv;
out vec4 color;
uniform sampler2D camera_texture;

void main() {
    color = texture(camera_texture, uv);
}
