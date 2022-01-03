#version 330 core
layout(location = 0) in vec3 vertex;

uniform mat4 g_light;
uniform mat4 world;
void main() {
    gl_Position = g_light *world* vec4(vertex, 1.0);
}