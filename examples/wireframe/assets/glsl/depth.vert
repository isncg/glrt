#version 330 core
layout(location = 0) in vec3 vertex;

uniform mat4 camera;
void main() {
    gl_Position = camera* vec4(vertex, 1.0);
}