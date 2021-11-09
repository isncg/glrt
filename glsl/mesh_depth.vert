#version 330 core
layout(location = 0) in vec3 vertex;

uniform mat4 lightmat;

void main() {
    gl_Position = lightmat * vec4(vertex, 1.0);
}