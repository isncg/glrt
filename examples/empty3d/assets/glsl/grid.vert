#version 330 core
layout (location = 0) in vec2 vertex;
uniform float size;
uniform mat4 cam;
void main()
{
    vec2 v = vertex*size;
    gl_Position = cam*vec4(v.x, 0.0, v.y, 1.0);
}