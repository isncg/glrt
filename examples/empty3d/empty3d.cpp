#include "empty3d.h"
#include "../../utils/utils.h"
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_opengl3.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace example
{
	void Empty3D::InitHorizonGrid()
	{
		m_pGridShader = ShaderLib::Instance().Load(ASSETPATH("glsl/grid"));
		//gridShader.Load(ASSETPATH("glsl/grid.vert"), ASSETPATH("glsl/grid.frag"));
		std::vector<Vector2> buffer;
		for (int i = -50; i <= 50; i++)
		{
			buffer.push_back(Vector2{ -50, i });
			buffer.push_back(Vector2{ 50, i });
			buffer.push_back(Vector2{ i, -50 });
			buffer.push_back(Vector2{ i, 50 });
		}
		GLASSERT(glGenVertexArrays(1, &gridVAO));
		GLASSERT(glBindVertexArray(gridVAO));
		GLuint vbo;
		GLASSERT(glGenBuffers(1, &vbo));
		GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		signed   long long int bufsize = buffer.size() * sizeof(Vector2);
		GLASSERT(glBufferData(GL_ARRAY_BUFFER, bufsize, &buffer[0], GL_STATIC_DRAW));
		GLASSERT(glEnableVertexAttribArray(0));
		GLASSERT(glVertexAttribPointer(0,
			GetVectorComponentCount(buffer.front()),
			GetVectorComponentType(buffer.front()),
			GL_FALSE,
			sizeof(Vector2), 0));
		GLASSERT(glBindVertexArray(0));
		GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void Empty3D::InitAxis()
	{
		m_pAxisShader = ShaderLib::Instance().Load(ASSETPATH("glsl/axis"));
		//axisShader.Load(ASSETPATH("glsl/axis.vert"), ASSETPATH("glsl/axis.frag"));
		std::vector<Vector3> buffer;
		buffer.push_back(Vector3{ 0,0,0 }); buffer.push_back(Vector3{ 1,0,0 });
		buffer.push_back(Vector3{ 1,0,0 }); buffer.push_back(Vector3{ 1,0,0 });

		buffer.push_back(Vector3{ 0,0,0 }); buffer.push_back(Vector3{ 0,1,0 });
		buffer.push_back(Vector3{ 0,1,0 }); buffer.push_back(Vector3{ 0,1,0 });

		buffer.push_back(Vector3{ 0,0,0 }); buffer.push_back(Vector3{ 0,0,1 });
		buffer.push_back(Vector3{ 0,0,1 }); buffer.push_back(Vector3{ 0,0,1 });

		GLASSERT(glGenVertexArrays(1, &axis));
		GLASSERT(glBindVertexArray(axis));
		GLuint vbo;
		GLASSERT(glGenBuffers(1, &vbo));
		GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		auto bufsize = buffer.size() * sizeof(Vector3);
		GLASSERT(glBufferData(GL_ARRAY_BUFFER, bufsize, &buffer[0], GL_STATIC_DRAW));
		GLASSERT(glEnableVertexAttribArray(0));
		GLASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0));
		GLASSERT(glEnableVertexAttribArray(1));
		GLASSERT(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)sizeof(Vector3)));
		GLASSERT(glBindVertexArray(0));
		GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));		
	}

	LRESULT Empty3D::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
		return Window::WndProc(message, wParam, lParam);
	}

	void Empty3D::OnCreate()
	{
		Window::OnCreate();
		CameraStartupParam camparam;
		SetCameraStartupParam(camparam);
		m_Camera.SetProjectionMatrix(camparam.projection.fovY, GetClientAspect(), camparam.projection.zNear, camparam.projection.zFar);
		m_CamController.camera = &m_Camera;
		m_CamController.speed = camparam.moveSpeed;
		Vector3 initCamPos = camparam.position;
		Vector2 initCamYallPitch = Camera::GetYallPitchFromDirection(camparam.direction);
	/*	Vector3 initCamDir = camparam.direction;
		initCamDir = glm::normalize(initCamDir);
		vector
		float yall = glm::atan(initCamDir.z, initCamDir.x);*/
		m_CamController.Setup(initCamPos, initCamYallPitch.x, initCamYallPitch.y);

		InitHorizonGrid();
		InitAxis();

		GLASSERT(glEnable(GL_DEPTH_TEST));
		GLASSERT(glEnable(GL_BLEND));
		GLASSERT(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void Empty3D::OnDestroy()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}


	void Empty3D::GetInitSize(long* width, long* height) { *width = w; *height = h; }
	void Empty3D::OnMouseMove(long dx, long dy, long x, long y) { m_CamController.OnMouseMove(dx, dy, x, y); }
	void Empty3D::DrawHorizonGrid(float size)
	{
		GLASSERT(glDisable(GL_DEPTH_TEST));
		auto pos = m_CamController.position;
		Vector2 offset;
		offset.x = (long)(pos.x / size) * size;
		offset.y = (long)(pos.z / size) * size;
		m_pGridShader->Use();
		m_pGridShader->Set("cam", m_Camera.GetMatrix());
		m_pGridShader->Set("size", size);
		m_pGridShader->Set("viewpos", m_CamController.position);
		m_pGridShader->Set("gridcolor", Vector3{ 1,1,1 });
		m_pGridShader->Set("offset", offset);
		GLASSERT(glBindVertexArray(gridVAO));
		GLASSERT(glDrawArrays(GL_LINES, 0, 404));
		GLASSERT(glEnable(GL_DEPTH_TEST));
	}
	void Empty3D::DrawAxis()
	{
		GLASSERT(glDisable(GL_DEPTH_TEST));
		m_pAxisShader->Use();
		m_pAxisShader->Set("_mvp", 
			m_Camera.GetProjectionMatrix()*
			Matrix4x4::LookAt(-m_CamController.GetForwardDirection()*5.0f* m_Camera.projectionParam.zNear, Vector3{ 0,0,0 }, Vector3{0,1,0}));
		GLASSERT(glBindVertexArray(axis));
		GLASSERT(glDrawArrays(GL_LINES, 0, 12));
		GLASSERT(glEnable(GL_DEPTH_TEST));
	}
	void Empty3D::AfterRender()
	{
		Window::AfterRender();
		m_CamController.Update(frameStatics->GetDelta());
		GlobalMaterial::Instance().SetMainCamera(&m_Camera);
	}
	void Empty3D::SetCameraStartupParam(CameraStartupParam& param)
	{
		param.projection.fovY = 3.1415926f / 3;
		param.projection.zFar = 5000;
		param.projection.zNear = 1;

		param.position = Vector3{ 4,3,4 };
		param.direction = -param.position;
		param.moveSpeed = 10.0f;
	}
	void Empty3D::OnKeyboard(KEYS key, KEYACTION action) 
	{
		m_CamController.OnKeyboard(key, action); 
	}
	void Empty3D::Render()
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		DrawHorizonGrid(0.1f);
		DrawHorizonGrid(1.0f);
		DrawHorizonGrid(10.0f);
		DrawHorizonGrid(100.0f);
		
	}

	void Empty3D::RenderPipline()
	{
		GLASSERT(Render());
		GLASSERT(PostRender());
		GLASSERT(DrawAxis());

		GLASSERT(ImGui_ImplOpenGL3_NewFrame());
		GLASSERT(ImGui_ImplWin32_NewFrame());
		GLASSERT(ImGui::NewFrame());
		GLASSERT(OnGUI());
		GLASSERT(ImGui::Render());
		GLASSERT(ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()));
	}
}

//RUN_WINDOW(example::Empty3D)