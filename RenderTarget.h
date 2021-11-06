#pragma once
#include "framework.h"
#include "Renderer.h"
class RenderTarget
{
	GLuint fbo;
public:
	std::vector<Texture> colorTextures;
	Texture depthTexture;
	void Init(int width, int height, int nColors, bool depth);
	void Bind();
};

