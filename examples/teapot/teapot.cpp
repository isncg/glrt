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