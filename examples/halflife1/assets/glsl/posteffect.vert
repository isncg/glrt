#version 330 core
layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec2 uv2;

out vec2 frag_uv;
out vec2 frag_uv2;

void main()
{
    gl_Position = vec4(vertex, 0.0, 1.0);
    frag_uv = uv;
    frag_uv2 = uv2;
}