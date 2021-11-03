#version 330 core
in vec2 frag_uv;
in vec3 frag_pos;
in vec3 frag_norm;
out vec4 color;

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
    return max(0.4, min(0.9, s));
    return s;
}



void main()
{
    float v = gridv(frag_pos, 200.0);
    color = vec4(v,v,v,1.0  );
    //color = vec4(grid(frag_pos.x, 100.0),grid(frag_pos.y, 100.0),grid(frag_pos.z, 100.0),1.0  );
}