#version 330 core
layout(location = 0) in vec3 vertex;

uniform mat4 light;

void main() {
    gl_Position = light * vec4(vertex, 1.0);
}