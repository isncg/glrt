#version 330 core
in vec3 _color;
layout (location = 0) out vec4 color0;
void main() {
    color0 = vec4(_color, 1.0);
}