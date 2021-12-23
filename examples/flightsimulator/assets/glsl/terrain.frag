#version 330 core
in float height;
in vec3 norm;
out vec4 color;
in float flaten;
in vec2 uv;
in vec2 flatenuv;
uniform vec3 flatenColor = vec3(0.43, 0.35, 0.27);
uniform sampler2D rgbmap;
void main()
{
    // vec3 rgb = vec3(pow(height, 4), 1.0 - height, 1.0-0.5*height);
    // rgb*=dot(norm, vec3(0.0, 1.0, 0.0));
    vec3 l = normalize(vec3(1.0,1.0,1.0));
    float d = clamp(dot(norm, l), 0.0, 1.0)*0.5+0.5;
    //color = vec4(norm*0.5+vec3(0.5), 1.0); 
    color = vec4((texture2D(rgbmap, uv).rgb * (1.0 - flaten) + flatenColor * flaten) * d, 1.0); 
}