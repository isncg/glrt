#version 330 core
layout (location = 0) in vec2 vertex;
uniform float size;
uniform mat4 cam;
uniform vec2 offset;
out vec3 pos;
void main()
{
    vec2 v = vertex*size;
    pos = vec3(v.x + offset.x, 0.0, v.y + offset.y);
    gl_Position = cam*vec4(pos, 1.0);
}