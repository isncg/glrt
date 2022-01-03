#version 330 core
in vec2 frag_uv;
in vec3 frag_pos;
in vec3 frag_norm;
out vec4 color;

uniform sampler2D tex;
uniform sampler2D g_shadow;
uniform mat4 g_light;
uniform vec3 g_lightcolor;
uniform mat4 world;
uniform float g_shadowbias = 0.005;
uniform float g_ambient = 0.1;
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
vec3 getShadowColor(vec3 originalcolor) {
    float shadow = getShadow(frag_pos);
    float underlight = getCast(frag_pos);
    vec3 lightdir =  getViewMatrixForward(g_light);
    float luminance = max(g_ambient, (1.0 - shadow)*max(dot(frag_norm, lightdir), 0.0));
    vec3 shadowColor = originalcolor * (1.0 - underlight) + underlight * originalcolor * g_lightcolor;
    return shadowColor * luminance;
}





void main()
{
    vec3 lightdir =  getViewMatrixForward(g_light);
    float luminance = (1.0-g_ambient)*max(dot(frag_norm, lightdir), 0.0)+g_ambient;
    ivec2 texSize = textureSize(tex, 0);
    vec4 diffuse = texture(tex, vec2(frag_uv.x/texSize.x, frag_uv.y/texSize.y));
    color = vec4(getShadowColor(diffuse.rgb), diffuse.a);

}