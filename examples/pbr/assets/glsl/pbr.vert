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

uniform mat4 g_cam;
uniform mat4 world;

// out vec3 FragPos;																					 //输入世界坐标
// out vec3 Normal;																						 //输入法向量
// out vec2 TexCoord;	

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

void main()
{
    vec4 worldPos = world * vec4(vertex, 1.0);
    gl_Position = g_cam * worldPos;
    WorldPos = worldPos.xyz;
    TexCoords = uv;
    Normal =  (world * vec4(normal, 1.0)).xyz;
}
