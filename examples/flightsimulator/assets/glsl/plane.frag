#version 330 core
in vec2 frag_uv;
in vec3 frag_pos;
in vec3 frag_norm;
out vec4 color;
uniform sampler2D tex;
uniform vec3 diffuse;

uniform sampler2D g_shadow;
uniform mat4 g_light;
uniform mat4 world;
uniform float g_shadowbias = 0.0;
uniform vec3 lightcastcolor = vec3(1);
float getCast(vec3 pos)
{
    vec4 p = g_light * world * vec4(pos, 1.0);
    p.xyz /= p.w;
    p.xyz = p.xyz * 0.5 + 0.5;
    float d = max(sign(p.x), 0.0)*max(sign(1.0-p.x), 0.0)*max(sign(p.y), 0.0)*max(sign(1.0-p.y), 0.0);
    return d;
}

float getShadow(vec3 pos)
{
    vec4 p = g_light * world * vec4(pos, 1.0);
    p.xyz /= p.w;
    p.xyz = p.xyz * 0.5 + 0.5;
    //float castmask = max(sign(p.x), 0.0)*max(sign(1.0-p.x), 0.0)*max(sign(p.y), 0.0)*max(sign(1.0-p.y), 0.0);
    float f = 3.0;
    float castmask = max(f*(p.x), 0.0)*max(f*(1.0-p.x), 0.0)*max(f*(p.y), 0.0)*max(f*(1.0-p.y), 0.0);
    castmask = min(castmask, 1.0);
    float d = texture(g_shadow, p.xy).r;
    return castmask*max(sign(p.z - d - g_shadowbias), 0.0);
}

vec3 getShadowColor(vec3 originalcolor) {
    float shadow = getShadow(frag_pos);
    float underlight = getCast(frag_pos);
    float luminance = max(0.2, (1.0 - shadow));
    vec3 shadowColor = originalcolor * (1.0 - underlight) + underlight * originalcolor * lightcastcolor;
    return shadowColor * luminance;
}

void main()
{
    color = vec4(getShadowColor(texture(tex, frag_uv).rgb * diffuse), 1.0);
    //color = vec4(frag_pos * (0.5f + 0.5f * dot(frag_norm, vec3(1.0, 0.0, 0.0))), 1.0);
}