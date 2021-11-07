#version 330 core
in vec2 frag_uv;
in vec2 frag_uv2;

out vec4 color;
uniform sampler2D depth;
uniform vec2 clipRange = vec2(1.0, 10000.0);
void main()
{
    float zNear = clipRange.x;
    float zFar = clipRange.y;
    float r = texture2D(depth, frag_uv).r;
    float z = 2.0*zFar *zNear /(zFar+zNear-r*(zFar-zNear));
    float zv = (z-zNear) / (zFar - zNear);
    color = vec4(zv, zv, zv, 1);
}