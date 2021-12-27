#version 330 core
layout(location = 0) in vec3 vertex;

uniform mat4 lightmat;
uniform mat4 _mv;
void main() {
    gl_Position = lightmat *_mv* vec4(vertex, 1.0);
}