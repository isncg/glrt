#version 330 core

layout (location = 0) out vec4 color0;
uniform float brightness;

void main()
{
    color0 = vec4(brightness, brightness, brightness, 1.0);
}