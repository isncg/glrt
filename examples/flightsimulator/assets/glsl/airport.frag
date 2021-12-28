#version 330 core
in vec2 frag_uv;
in vec3 frag_pos;
in vec3 frag_norm;
out vec4 color;
uniform vec4 diffuse;

uniform sampler2D shadowmap;
uniform mat4 lightview;
uniform mat4 world;

float getCast(vec3 pos)
{
    vec4 p = lightview * world * vec4(pos, 1.0);
    p.xyz /= p.w;
    p.xyz = p.xyz * 0.5 + 0.5;
    float d = max(sign(p.x), 0.0)*max(sign(1.0-p.x), 0.0)*max(sign(p.y), 0.0)*max(sign(1.0-p.y), 0.0);
    return d;
}

float getShadow(vec3 pos)
{
    vec4 p = lightview * world * vec4(pos, 1.0);
    p.xyz /= p.w;
    p.xyz = p.xyz * 0.5 + 0.5;
    //float castmask = max(sign(p.x), 0.0)*max(sign(1.0-p.x), 0.0)*max(sign(p.y), 0.0)*max(sign(1.0-p.y), 0.0);
    float f = 3.0;
    float castmask = max(f*(p.x), 0.0)*max(f*(1.0-p.x), 0.0)*max(f*(p.y), 0.0)*max(f*(1.0-p.y), 0.0);
    castmask = min(castmask, 1.0);
    float d = texture(shadowmap, p.xy).r;
    return castmask*max(sign(p.z - d - 0.00001), 0.0);
}

vec3 getShadowColor(vec3 originalcolor) {
    float shadow = getShadow(frag_pos);
    float underlight = getCast(frag_pos);
    float luminance = max(0.2, (1.0 - shadow));
    vec3 shadowColor = originalcolor * (1.0 - underlight) + underlight * originalcolor * vec3(0.9, 1.1, 1.1);
    return shadowColor * luminance;
}

vec3 getViewMatrixForward(mat4 matrix)
{
    vec3 forward;
    forward.x = -matrix[2][0];
    forward.y = -matrix[2][1];
    forward.z = -matrix[2][2];

    // forward.x = matrix[0][2];
    // forward.y = matrix[1][2];
    // forward.z = matrix[2][2];
    return normalize(forward);
}



void main()
{
    // float shadow = getShadow(frag_pos);
    // float underlight = getCast(frag_pos);
    // vec3 outputdiffuse = diffuse.rgb*(1.0-underlight)+underlight* diffuse.rgb*vec3(0.9, 1.1, 1.1);
    // float luminance = max(0.2, (1.0 - shadow));
    color = vec4(getShadowColor(diffuse.rgb), diffuse.a);
    //color = vec4(frag_pos * (0.5f + 0.5f * dot(frag_norm, vec3(1.0, 0.0, 0.0))), 1.0);
}