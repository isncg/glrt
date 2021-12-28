#version 330 core
in float height;
in vec3 norm;
out vec4 color;
in float flaten;
in vec2 uv;
in vec2 flatenuv;
in vec3 frag_pos;
uniform vec3 flatenColor = vec3(0.43, 0.35, 0.27);
uniform sampler2D rgbmap;


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



void main()
{
    vec3 l = normalize(vec3(1.0,1.0,1.0));
    float d = clamp(dot(norm, l), 0.0, 1.0)*0.5+0.5;
    color = vec4(getShadowColor((texture2D(rgbmap, uv).rgb * (1.0 - flaten) + flatenColor * flaten) * d), 1.0); 
}