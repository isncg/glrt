# GLRT
An OpenGL Rendering Toolkit


## Example 1: Hello, triangle!
```c++
#include "../../include/GLRT.h"
#include "../../utils/utils.h"
namespace example
{
	class HelloTriangle :public Window
	{
		ASSETDIR("examples/hellotriangle/assets/");
		Shader shader;
		MeshRenderer renderer;
		virtual void OnCreate() override
		{
			Window::OnCreate();
			shader.Load(ASSETPATH("glsl/mesh.vert"), ASSETPATH("glsl/mesh.frag"));
			Mesh mesh;
			mesh.vertices.push_back({-0.5, -0.75, 0 });  mesh.colors.push_back({ 1.0, 0.0, 0.0, 1.0 });
			mesh.vertices.push_back({ 0.5, -0.75, 0 });	 mesh.colors.push_back({ 0.0, 1.0, 0.0, 1.0 });
			mesh.vertices.push_back({ 0,    0.75, 0 });	 mesh.colors.push_back({ 0.0, 0.0, 1.0, 1.0 });

			mesh.triangles.push_back(0);  mesh.triangles.push_back(1);  mesh.triangles.push_back(2);
			renderer.Set(&mesh);
		}

		virtual void Render() override
		{
			glClearColor(0.2, 0.2, 0.2, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLASSERT(renderer.Draw(shader));
		}
	};
}
RUN_WINDOW(example::HelloTriangle)

```
![](https://github.com/isncg/.img/blob/master/glrt/hellotriangle.png)

## Example 2: Teapot
```c++
#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"
namespace example
{
	class Teapot :public Empty3D
	{
		ASSETDIR("examples/teapot/assets/");
		Shader shader;
		MeshRenderer renderer;
		Model model;
		virtual void OnCreate() override
		{
			Empty3D::OnCreate();
			model.Load(ASSETPATH("teapot.obj"));
			shader.Load(ASSETPATH("glsl/mesh.vert"), ASSETPATH("glsl/mesh.frag"));
			renderer.Set(&model.meshCollection.front());
		}

		virtual void Render() override
		{
			Empty3D::Render();
			shader.Set("camera", m_Camera.GetMatrix());
			GLASSERT(renderer.Draw(shader));
		}
	};
}
RUN_WINDOW(example::Teapot)
```
![](https://github.com/isncg/.img/blob/master/glrt/teapot.png)