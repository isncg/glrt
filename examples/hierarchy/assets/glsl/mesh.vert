#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 tangent;
layout (location = 3) in vec2 uv;
layout (location = 4) in vec2 uv2;
layout (location = 5) in vec2 uv3;
layout (location = 6) in vec2 uv4;
layout (location = 7) in vec2 uv5;
layout (location = 8) in vec2 uv6;
layout (location = 9) in vec2 uv7;
layout (location = 10) in vec2 uv8;
layout (location = 11) in vec4 color;
layout (location = 12) in ivec4 color32;

uniform mat4 camera;
uniform mat4 world;
out vec2 frag_uv;
out vec3 frag_pos;
out vec3 frag_norm;

void main()
{
    gl_Position = camera*world*vec4(vertex, 1.0);
    frag_uv = uv;
    frag_pos = vertex;
    frag_norm = normal;
}