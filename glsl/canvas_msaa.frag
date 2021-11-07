#version 330 core
in vec2 frag_uv;
in vec2 frag_uv2;

out vec4 color;
uniform sampler2D tex;
uniform float ssize;
uniform float blur;

uint wseed;
uint whash(uint seed)
{
    seed = (seed ^ uint(61)) ^ (seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> uint(4));
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> uint(15));
    return seed;
}

float rand()
{
	wseed = whash(wseed);
	return float(wseed) * (1.0 / 4294967296.0);
}



void main()
{
    vec4 sum = vec4(0);
    ivec2 textureSize2d = textureSize(tex,0);
    vec2 pixelSize = vec2(1.0/textureSize2d.x, 1.0/textureSize2d.y);
    for(int i=0;i<ssize;i++)
    {
        sum+=texture2D(tex, frag_uv+blur*vec2(pixelSize.x*rand(), pixelSize.y*rand()));
    }
    color = sum/ssize;
}