#version 330 core
in vec2 frag_uv;
in vec3 frag_pos;
in vec3 frag_norm;
out vec4 color;
uniform vec4 diffuse;

uniform sampler2D shadowmap;
uniform mat4 lightmat;
uniform mat4 _mv;

void main()
{
    vec4 map = lightmat *_mv* vec4(frag_pos, 1.0);
    map/=map.w;
    
    float lightdepth = texture2D(shadowmap, map.xy*0.5+vec2(0.5, 0.5)).r;

    float pixeldepth = map.z*0.5+0.5;
    vec3 lightdir;
    lightdir.x= lightmat[0][2];
    lightdir.y= lightmat[1][2];
    lightdir.z= lightmat[2][2];
    // float dd = 0.5*dot(frag_norm, normalize(-lightdir));
    // if(dd<0)
    //     dd = 0;
    float mask = 1;
    if(pixeldepth>lightdepth+0.00001)
        mask=0.2;
    //float emmi = 0.5;
    //float l = dd*direct+emmi;
	color = vec4(diffuse.rgb*mask, diffuse.a);
    //color = vec4(frag_pos * (0.5f + 0.5f * dot(frag_norm, vec3(1.0, 0.0, 0.0))), 1.0);
}