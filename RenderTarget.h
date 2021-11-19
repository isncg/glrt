#pragma once
#include "framework.h"
#include "Renderer.h"
class RenderTarget
{
	GLuint fbo;
	glm::ivec2 _size;
public:
	Vector4 clearColor{ 0.2f, 0.2f, 0.2f, 1.0f };
	GLbitfield clearBit = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	int passID;
	int width();
	int height();
	const glm::ivec2& size();
	std::vector<Texture> colorTextures;
	Texture depthTexture;
	void Init(int width, int height, int nColors, bool depth);
	void Bind(int passID = -1);
	void Draw(Renderer& renderer);
};

