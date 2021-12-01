#include "empty3d.h"
#include "../../utils/utils.h"

namespace example
{
	void Empty3D::OnCreate()
	{
		// grid
		std::vector<Vector2> buffer;
		for (int i = -50; i <= 50; i++)
		{
			buffer.push_back(Vector2{ -50, i });
			buffer.push_back(Vector2{ 50, i });
			buffer.push_back(Vector2{ i, -50 });
			buffer.push_back(Vector2{ i, 50 });
		}
		GLASSERT(glGenVertexArrays(1, &grid));
		GLASSERT(glBindVertexArray(grid));
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


	void Empty3D::GetInitSize(long* width, long* height) { *width = w; *height = h; }
	void Empty3D::OnMouseMove(long dx, long dy, long x, long y) { m_CamController.OnMouseMove(dx, dy, x, y); }
	void Empty3D::DrawHorizonGrid(float size, float brightness)
	{
		gridShader.Set("cam", m_Camera.GetMatrix());
		gridShader.Use();
		GLASSERT(glBindVertexArray(grid));
		GLASSERT(glDrawArrays(GL_LINES, 0, 200));
	}
	void Empty3D::AfterRender()
	{
		m_CamController.Update();
	}
	void Empty3D::OnKeyboard(KEYS key, KEYACTION action) { m_CamController.OnKeyboard(key, action); }
}