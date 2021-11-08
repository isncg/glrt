#version 330 core
in vec2 frag_uv;
in vec3 frag_pos;
in vec3 frag_norm;
out vec4 color;

uniform sampler2D tex;
uniform sampler2D shadowmap;
uniform mat4 shadowcast;

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
    //vec2 uvmod = mod(frag_uv*0.01, 1.0);
    //float v = gridv(frag_pos, 200.0);
    //color = vec4(v,v,v,1.0  );
    vec4 map = shadowcast * vec4(frag_pos, 1.0);
    map/=map.w;
    float depth = texture2D(shadowmap, map.xy).r;    

    float l = dot(frag_norm, vec3(0.4,0.5,0.7));
    l = l*0.4;
    l+=0.6;
    // if(map.z>depth)
    //     l = 0;
    color = vec4(texture(tex, frag_uv).rgb* l, 1.0);
    //color = vec4(grid(frag_pos.x, 100.0),grid(frag_pos.y, 100.0),grid(frag_pos.z, 100.0),1.0  );
}