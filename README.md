# GLRT
A OpenGL Rendering Toolkit


## Sample: drawing a triangle
```c++
#include "framework.h"
#include "Application.h"
#include "Window.h"
#include "Renderer.h"
#include "utils/glhelpers.h"
class MyWindow :public Window
{
	Shader shader;
	Mesh mesh;
	MeshRenderer renderer;
	
	virtual void OnCreate() override
	{
		Window::OnCreate();

		shader.Load("glsl/mesh.vert", "glsl/mesh.frag");
		shader.Use();

		mesh.vertices.push_back(Vector3{ -0.5,-0.75,0 });
		mesh.vertices.push_back(Vector3{ 0.5,-0.75,0 });
		mesh.vertices.push_back(Vector3{ 0,   0.75,0 });

		mesh.uv.push_back(Vector2{ 0,  0 });
		mesh.uv.push_back(Vector2{ 1,  0 });
		mesh.uv.push_back(Vector2{ 0.5,1 });

		mesh.triangles.push_back(0);
		mesh.triangles.push_back(1);
		mesh.triangles.push_back(2);

		renderer.Set(&mesh);
		ready = true;

		glAssert("oncreate finish");
	}

	virtual void Render() override
	{
		glClearColor(0.2, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.Use();
		GLASSERT(renderer.Draw());
	}
};

RUN_WINDOW(MyWindow)

```
![](https://github.com/isncg/.img/blob/master/glrt/glrt.png)
