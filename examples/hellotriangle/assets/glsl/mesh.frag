#version 330 core

layout (location = 0) out vec4 color0;
in vec4 frag_color;

void main()
{
    color0 = frag_color;
}