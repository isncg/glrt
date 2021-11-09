#version 330 core
in vec2 frag_uv;
in vec3 frag_pos;
in vec3 frag_norm;
out vec4 color;

uniform sampler2D tex;
uniform sampler2D shadowmap;
uniform mat4 lightmat;
uniform vec3 lightdir;

float grid_raw(float v, float m)
{
    return (mod(v, m) - 0.5*m)*1000.0 + 0.5;
}


float grid(float v, float m)
{
    return max(0.4, min(0.6, (mod(v, m) - 0.5*m)*1000.0 + 0.5));
}

float gridv(vec3 v, float m)
{
    float s = grid_raw(v.x, m)*grid_raw(v.y, m)*grid_raw(v.z, m);
    return max(0.0, min(1.0, s)) * (0.8+0.2*dot(frag_norm, vec3(1.0,0.0,0.0)));
}



void main()
{
    vec4 map = lightmat * vec4(frag_pos, 1.0);
    map/=map.w;
    
    float lightdepth = texture2D(shadowmap, map.xy*0.5+vec2(0.5, 0.5)).r;
    float pixeldepth = map.z*0.5+0.5;

    float diffuse = 0.5*dot(frag_norm, normalize(-lightdir));
    if(diffuse<0)
        diffuse = 0;
    float direct = 1;
    if(pixeldepth>lightdepth+0.00001)
        direct=0;
    float emmi = 0.5;
    float l = diffuse*direct+emmi;
//    l+=0.6;
//    if(pixeldepth>lightdepth+0.00001)
//         l *=0.5;
    color = vec4(texture(tex, frag_uv).rgb* l, 1.0);

}