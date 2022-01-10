#version 330 core
in vec2 frag_uv;
in vec3 frag_pos;
in vec3 frag_norm;
out vec4 color;

void main()
{
    color = vec4(0.0,1.0,0.0, 1.0);
}