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
//RUN_WINDOW(example::HelloTriangle)