#pragma once
#include "../../include/GLRT.h"
namespace example
{
	class Empty3D : Window
	{
		Shader gridShader;
		GLuint grid = 0;
		GLuint axis = 0;
		GLuint skybox = 0;
		long w = 1280, h = 720;
	public:
		Camera						m_Camera;
		CameraFirstPersonController m_CamController;
		virtual void OnCreate() override;
		virtual void GetInitSize(long* width, long* height) override;
		virtual void OnMouseMove(long dx, long dy, long x, long y) override;
		virtual void OnKeyboard(KEYS key, KEYACTION action) override;

		void DrawSkyboxBg();
		void DrawAxis();
		void DrawHorizonGrid(float size, float brightness);

		virtual void AfterRender() override;
	};
}