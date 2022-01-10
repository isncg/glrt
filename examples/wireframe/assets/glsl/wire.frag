#version 330 core
uniform mat4 camera;
uniform sampler2D depthbuf;

in vec3 frag_pos;
flat in vec3 vert_pos;
uniform float scatter_size = 0.2;

out vec4 color;

float getShadow(vec3 pos) {
    vec4 p = camera * vec4(pos, 1.0);
    p.xyz /= p.w;
    p.xyz = p.xyz * 0.5 + 0.5;
    float f = 3.0;
    float castmask = max(f * (p.x), 0.0) * max(f * (1.0 - p.x), 0.0) * max(f * (p.y), 0.0) * max(f * (1.0 - p.y), 0.0);
    castmask = min(castmask, 1.0);
    float d = texture(depthbuf, p.xy).r;
    return castmask * max(sign(p.z - d - 0.00001), 0.0);
}

void main() {
    float shadow = getShadow(frag_pos);
    float distance = length(vert_pos - frag_pos);
    float alpha = 1.0 - shadow + shadow * max(0.0, sign(mod(distance, scatter_size) - scatter_size*0.5));
    color = vec4(1.0, 1.0, 1.0, alpha);
}