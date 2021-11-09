#version 330 core
in vec2 frag_uv;
in vec3 frag_pos;
in vec3 frag_norm;
out vec4 color;

uniform sampler2D tex;
uniform sampler2D shadowmap;
uniform mat4 lightmat;

void main()
{
    vec4 map = lightmat * vec4(frag_pos, 1.0);
    map/=map.w;
    
    float lightdepth = texture2D(shadowmap, map.xy*0.5+vec2(0.5, 0.5)).r;

    float pixeldepth = map.z*0.5+0.5;
    vec3 lightdir;
    lightdir.x= lightmat[0][2];
    lightdir.y= lightmat[1][2];
    lightdir.z= lightmat[2][2];
    float diffuse = 0.5*dot(frag_norm, normalize(-lightdir));
    if(diffuse<0)
        diffuse = 0;
    float direct = 1;
    if(pixeldepth>lightdepth+0.00001)
        direct=0;
    float emmi = 0.5;
    float l = diffuse*direct+emmi;
    color = vec4(texture(tex, frag_uv).rgb* l, 1.0);

}