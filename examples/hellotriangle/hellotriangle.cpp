#include "../../include/GLRT.h"
#include "../../utils/utils.h"

namespace example
{
	// assets/
	//   glsl/
	//     mesh.vert
	//     mesh.frag
	
	class HelloTriangle :public Window
	{
		Shader shader;
		Mesh mesh;
		MeshRenderer renderer;

		virtual void OnCreate() override
		{
			Window::OnCreate();

			shader.Load(GetAssetPath("glsl/mesh.vert"), GetAssetPath("glsl/mesh.frag"));

			mesh.vertices.push_back(Vector3{ -0.5,-0.75,0 });
			mesh.vertices.push_back(Vector3{ 0.5,-0.75,0 });
			mesh.vertices.push_back(Vector3{ 0,   0.75,0 });

			mesh.colors.push_back(Color{ 1.0, 0.0, 0.0, 1.0 });
			mesh.colors.push_back(Color{ 0.0, 1.0, 0.0, 1.0 });
			mesh.colors.push_back(Color{ 0.0, 0.0, 1.0, 1.0 });

			mesh.triangles.push_back(0);
			mesh.triangles.push_back(1);
			mesh.triangles.push_back(2);

			renderer.Set(&mesh);
			mesh.Clear();
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

}
RUN_WINDOWEX(example::HelloTriangle, "examples/hellotriangle/assets/")