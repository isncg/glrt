#pragma once
#include "framework.h"
#include "Renderer.h"
#include "Window.h"

class IViewport
{
public:
	virtual void GetViewportSize(Vector2* pOutSize) = 0;
};


class RenderPass
{
	GLuint fbo = 0;
	glm::ivec2 _size;
public:
	Vector4 clearColor{ 0.2f, 0.2f, 0.2f, 1.0f };
	GLbitfield clearBit = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	//int passID;
	int width();
	int height();
	const glm::ivec2& size();
	std::vector<std::shared_ptr<Texture>> colorBuffers;
	Texture depthBuffer;
	void Init(int width, int height, int nColors, bool depth);
	void Bind();
	static void BindDefault(Window* pWindow);
	static void BindDefault(IViewport* pViewport);
};