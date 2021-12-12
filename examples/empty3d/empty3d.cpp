#include "empty3d.h"
#include "../../utils/utils.h"

namespace example
{
	void Empty3D::InitHorizonGrid()
	{
		gridShader.Load(ASSETPATH("glsl/grid.vert"), ASSETPATH("glsl/grid.frag"));
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
		int bufsize = buffer.size() * sizeof(Vector2);
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
		axisShader.Load(ASSETPATH("glsl/axis.vert"), ASSETPATH("glsl/axis.frag"));
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
		int bufsize = buffer.size() * sizeof(Vector3);
		GLASSERT(glBufferData(GL_ARRAY_BUFFER, bufsize, &buffer[0], GL_STATIC_DRAW));
		GLASSERT(glEnableVertexAttribArray(0));
		GLASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0));
		GLASSERT(glEnableVertexAttribArray(1));
		GLASSERT(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)sizeof(Vector3)));
		GLASSERT(glBindVertexArray(0));
		GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void Empty3D::OnCreate()
	{
		Window::OnCreate();
		m_Camera.SetProjectionMatrix(3.1415926 / 3, (float)w / (float)h, 1, 5000);
		m_CamController.camera = &m_Camera;
		m_CamController.speed = 0.2f;
		Vector3 initCamPos{ 4,3,4 };
		m_CamController.Setup(initCamPos, -3 * glm::pi<float>() / 4,
			glm::atan(
				-initCamPos.y /
				glm::sqrt(glm::pow(initCamPos.x, 2) + glm::pow(initCamPos.z, 2))
			)
		);

		InitHorizonGrid();
		InitAxis();

		GLASSERT(glEnable(GL_DEPTH_TEST));
		GLASSERT(glEnable(GL_BLEND));
		GLASSERT(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
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
		gridShader.Use();
		gridShader.Set("cam", m_Camera.GetMatrix());
		gridShader.Set("size", size);
		gridShader.Set("viewpos", m_CamController.position);
		gridShader.Set("gridcolor", Vector3{ 1,1,1 });
		gridShader.Set("offset", offset);
		GLASSERT(glBindVertexArray(gridVAO));
		GLASSERT(glDrawArrays(GL_LINES, 0, 404));
		GLASSERT(glEnable(GL_DEPTH_TEST));
	}
	void Empty3D::DrawAxis()
	{
		GLASSERT(glDisable(GL_DEPTH_TEST));
		axisShader.Use();
		axisShader.Set("_mvp", 
			m_Camera.GetProjectionMatrix()*
			Matrix4x4::LookAt(-m_CamController.GetForwardDirection()*5.0f, Vector3{ 0,0,0 }, Vector3{0,1,0}));
		GLASSERT(glBindVertexArray(axis));
		GLASSERT(glDrawArrays(GL_LINES, 0, 12));
		GLASSERT(glEnable(GL_DEPTH_TEST));
	}
	void Empty3D::AfterRender()
	{
		m_CamController.Update();
	}
	void Empty3D::OnKeyboard(KEYS key, KEYACTION action) 
	{
		m_CamController.OnKeyboard(key, action); 
	}
	void Empty3D::Render()
	{
		glClearColor(0.2, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		DrawHorizonGrid(0.1f);
		DrawHorizonGrid(1.0f);
		DrawHorizonGrid(10.0f);
		DrawHorizonGrid(100.0f);
		
	}

	void Empty3D::PostRender()
	{
		DrawAxis();
	}
}

//RUN_WINDOW(example::Empty3D)