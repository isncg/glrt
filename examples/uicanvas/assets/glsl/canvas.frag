#version 330 core
in vec2 frag_uv;
in vec2 frag_uv2;

out vec4 color;
uniform sampler2D tex;
void main()
{
    color = texture2D(tex, frag_uv);//vec4(frag_uv, 0.0, 1.0);
}