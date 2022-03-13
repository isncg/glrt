#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;

uniform mat4 _mvp;
out vec3 _color;
void main()
{
    gl_Position = _mvp*vec4(vertex, 1.0);
    _color = color;
}