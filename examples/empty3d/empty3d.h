#pragma once
#include "../../include/GLRT.h"
namespace example
{
	class Empty3D : public Window
	{
		Shader gridShader;
		GLuint grid = 0;
		GLuint axis = 0;
		GLuint skybox = 0;
		long w = 1280, h = 720;

		// teapot
		Shader shader;
		MeshRenderer renderer;
		Model model;
		int frame = 0;
		Matrix4x4 mat = Matrix4x4(1);

		void InitHorizonGrid();
	public:
		Camera						m_Camera;
		CameraFirstPersonController m_CamController;
		virtual void OnCreate() override;
		virtual void GetInitSize(long* width, long* height) override;
		virtual void OnMouseMove(long dx, long dy, long x, long y) override;
		virtual void OnKeyboard(KEYS key, KEYACTION action) override;
		virtual void Render() override;
		void DrawSkyboxBg();
		void DrawAxis();
		void DrawHorizonGrid(float size);

		virtual void AfterRender() override;
	};
}