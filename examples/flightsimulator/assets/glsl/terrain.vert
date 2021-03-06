#version 330 core
layout(location = 0) in vec3 vertex;
uniform sampler2D hightmap;
uniform sampler2D flatenmap;
uniform vec3 flatenPos = vec3(-100.0, 0.17, -200.0);
uniform vec2 flatenSize = vec2(100.0, 100.0);
//uniform mat4 cam;
uniform float heightScale = 200.0;
uniform float meshScale = 1000.0;
out float height;
out vec3 norm;
out float flaten;
out vec2 uv;
out vec2 flatenuv;
out vec3 frag_pos;
uniform float d = 0.002;
uniform mat4 g_cam;
uniform mat4 world;

float getHeight(vec2 zx) {
    vec2 uv = zx * 0.5 + vec2(0.5, 0.5);
    float height = texture(hightmap, uv).r;
    float px = zx.x * meshScale;
    float pz = zx.y * meshScale;
    vec2 flatenuv = clamp(vec2((pz - flatenPos.z) / flatenSize.x, (px - flatenPos.x) / flatenSize.y), vec2(0.0, 0.0), vec2(1.0, 1.0));
    float flaten = texture(flatenmap, flatenuv).r;
    float py = (height * (1.0 - flaten) + flaten * flatenPos.y) * heightScale;
    return py;
}

void main() {
    vec2 zx = vertex.zx;
    uv = zx * 0.5 + vec2(0.5, 0.5);
    vec2 worldZX = zx * meshScale;
    height = getHeight(zx);
    float px = zx.x * meshScale;
    float pz = zx.y * meshScale;
    flatenuv = clamp(vec2((pz - flatenPos.z) / flatenSize.x, (px - flatenPos.x) / flatenSize.y), vec2(0.0, 0.0), vec2(1.0, 1.0));
    flaten = texture(flatenmap, flatenuv).r;
    float heightdz = getHeight(vec2(zx.x + d, zx.y)) - getHeight(vec2(zx.x - d, zx.y));
    float heightdx = getHeight(vec2(zx.x, zx.y + d)) - getHeight(vec2(zx.x, zx.y - d));
    frag_pos = vec3(worldZX.y, getHeight(zx), worldZX.x);
    gl_Position = g_cam * world * vec4(frag_pos, 1.0);
    norm = normalize(vec3(-heightdx, d * meshScale, -heightdz));
}