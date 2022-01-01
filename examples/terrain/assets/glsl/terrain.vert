#version 330 core
layout (location = 0) in vec3 vertex;
uniform sampler2D hightmap;
uniform mat4 g_cam;
uniform float heightScale = 1.0;
uniform vec2 meshScale = vec2(10.0, 10.0);
out float height;
out vec3 norm;
void main()
{
    float d = 0.001;
	float dfactor = 0.5;
    vec2 dx = vec2(dfactor*d, 0.0);
    vec2 dz = vec2(0.0, dfactor*d);
    height = texture(hightmap, vertex.zx).r;
    float heightx = texture(hightmap, vertex.zx+dx).r;
    float heightz = texture(hightmap, vertex.zx+dz).r;
    float heightx2 = texture(hightmap, vertex.zx-dx).r;
    float heightz2 = texture(hightmap, vertex.zx-dz).r;
    //height = (heightx + heightz + height) / 3.0;
    gl_Position = g_cam * vec4(vertex.x * meshScale.x-meshScale.x*0.5, height * heightScale, vertex.z * meshScale.y-meshScale.y*0.5, 1.0);
    norm = normalize(vec3(-heightScale*0.5*(heightx-heightx2), d*length(meshScale), -heightScale*0.5*(heightz-heightx2)));
}