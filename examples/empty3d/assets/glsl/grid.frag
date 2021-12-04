#version 330 core

layout (location = 0) out vec4 color0;

uniform vec3 gridcolor;
uniform vec3 viewpos;
uniform float size;
in vec3 pos;
void main()
{
    vec3 dir = pos - viewpos;
    float distsqr = dot(dir, dir);
    float alpha = min(100 * pow(size, exp(-size) + 1) / distsqr, 0.5);
    color0 = vec4(gridcolor, alpha);
    //color0 = vec4(1.0,1.0,1.0, 1.0);
}