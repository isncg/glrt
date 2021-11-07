#pragma once
#include "framework.h"
#include "Renderer.h"
class RenderTarget
{
	GLuint fbo;
	glm::ivec2 _size;
public:
	int width();
	int height();
	const glm::ivec2& size();
	std::vector<Texture> colorTextures;
	Texture depthTexture;
	void Init(int width, int height, int nColors, bool depth);
	void Bind();
};

