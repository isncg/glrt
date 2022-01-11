#pragma once
#include "framework.h"
#include "Renderer.h"
#include "Window.h"
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
	//void Draw(Renderer& renderer);
	//void Draw(Renderer& renderer, Shader& shader);

	//template<typename T> // where T: Renderer
	//void Draw(std::vector<T>& renderers);

	//template<typename T> // where T: Renderer
	//void Draw(std::vector<T>& renderers, Material& mat);

	//template<typename T> // where T: Renderer
	//void Draw(std::vector<T>& renderers, Shader& shader);
};
//
//template<typename T>
//inline void RenderPass::Draw(std::vector<T>& renderers)
//{
//	for (auto& r : renderers)
//	{
//		r.material->Use();
//		r.Draw();
//	}
//}
//
//template<typename T>
//inline void RenderPass::Draw(std::vector<T>& renderers, Material& mat)
//{
//	mat.Use();
//	for (auto& r : renderers)
//		r.Draw();
//}
//
//template<typename T>
//inline void RenderPass::Draw(std::vector<T>& renderers, Shader& shader)
//{
//	shader.Use();
//	for (auto& r : renderers)
//		r.Draw();
//}
