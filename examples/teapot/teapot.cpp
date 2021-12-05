#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"

namespace example
{
	class Teapot :public Empty3D
	{
	protected:
		ASSETDIR("examples/teapot/assets/");
		Shader shader;
		MeshRenderer renderer;
		Model model;
		virtual void OnCreate() override
		{
			Empty3D::OnCreate();
			LoadModel(&model, ASSETPATH("teapot.obj"));
			shader.Load(ASSETPATH("glsl/mesh.vert"), ASSETPATH("glsl/mesh.frag"));
			shader.Use();
			renderer.Set(&model.meshCollection.front());
		}

		virtual void Render() override
		{
			Empty3D::Render();
			shader.Use();
			shader.Set("_mvp", m_Camera.GetMatrix());
			GLASSERT(renderer.Draw());
		}
	};
}
//RUN_WINDOW(example::Teapot)