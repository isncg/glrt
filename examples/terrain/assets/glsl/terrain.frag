#version 330 core
in float height;
in vec3 norm;
out vec4 color;

void main()
{
    // vec3 rgb = vec3(pow(height, 4), 1.0 - height, 1.0-0.5*height);
    // rgb*=dot(norm, vec3(0.0, 1.0, 0.0));
    vec3 l = normalize(vec3(1.0,1.0,1.0));
    float d = dot(norm, l);
    //color = vec4(norm*0.5+vec3(0.5), 1.0); 
    color = vec4(vec3(d), 1.0); 
}