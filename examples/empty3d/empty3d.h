#pragma once
#include "../../include/GLRT.h"

struct ImGuiContext;

namespace example
{
	struct CameraStartupParam
	{
		CameraProjectionParam projection;
		Vector3 position;
		Vector3 direction;
		float moveSpeed;
	};


	class Empty3D : public Window
	{
		ASSETDIR("examples/empty3d/assets/");
		Shader* m_pGridShader;
		Shader* m_pAxisShader;
		GLuint gridVAO = 0;
		GLuint axis = 0;
		GLuint skybox = 0;
		long w = 1280, h = 720;

		// teapot
		//Shader shader;
		//MeshRenderer renderer;
		//Model model;
		//int frame = 0;
		//Matrix4x4 mat = Matrix4x4(1);
		ImGuiContext* imgui_ctx;
		void InitHorizonGrid();
		void InitAxis();
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	public:
		Camera						m_Camera;
		CameraFirstPersonController m_CamController;
		virtual void OnCreate() override;
		virtual void OnDestroy() override;
		virtual void GetInitSize(long* width, long* height) override;
		virtual void OnMouseMove(long dx, long dy, long x, long y) override;
		virtual void OnKeyboard(KEYS key, KEYACTION action) override;
		virtual void Render() override;
		virtual void RenderPipline() override;
		void DrawSkyboxBg();
		void DrawAxis();
		void DrawHorizonGrid(float size);
		virtual void AfterRender() override;
	protected:
		virtual void SetCameraStartupParam(CameraStartupParam& param);
	};
}