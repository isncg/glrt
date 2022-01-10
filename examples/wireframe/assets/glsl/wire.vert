#version 330 core
layout(location = 0) in vec3 vertex;

uniform mat4 camera;
out vec3 frag_pos;
flat out vec3 vert_pos;
void main() {
    gl_Position = camera* vec4(vertex, 1.0);
    vert_pos = vertex;
    frag_pos = vertex;
}