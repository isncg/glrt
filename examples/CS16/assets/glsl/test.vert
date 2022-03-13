#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 3) in vec2 uv;

out vec2 frag_uv;
void main()
{
    gl_Position = vec4(vertex.xy*vertex.z, 0.0, vertex.z);
    frag_uv = uv;
}