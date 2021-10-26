#include "framework.h"
#include "Application.h"
#include "Window.h"
#include "Renderer.h"
#include "utils/glhelpers.h"
#include "utils/asset_loader.h"

class Sample_ModelLoadingAndRendering :public Window
{
	Shader shader;
	MeshRenderer renderer;
	Model model;
	virtual void OnCreate() override
	{
		Window::OnCreate();
		LoadModel(&model, "assets/teapot.obj");
		shader.Load("glsl/mesh.vert", "glsl/mesh.frag");
		shader.Use();

		renderer.Set(&model.meshCollection.front());
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

//RUN_WINDOW(Sample_ModelLoadingAndRendering)
