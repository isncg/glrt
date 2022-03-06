#include "../../include/GLRT.h"
#include <utils/utils.h>

namespace example
{
	class UICanvas :public Window
	{
		ASSETDIR("examples/uicanvas/assets/");
		Shader shader;
		CanvasMesh canvasMesh;
		Texture texture;
		CanvasRenderer renderer;
		virtual void OnCreate() override
		{
			Window::OnCreate();
			texture.Load(ASSETPATH("lena.jpg"));
			shader.Load(ASSETPATH("glsl/canvas.vert"), ASSETPATH("glsl/canvas.frag"));
			shader.Set("tex", texture);
			shader.Use();
			auto hsize = GetViewportSize()*0.5f;
			canvasMesh.MergeBatch(CanvasRect({ -hsize.x + 5,   hsize.y - 5   }, { 512,512 }, { 0,1 }), hsize);
			canvasMesh.MergeBatch(CanvasRect({ -hsize.x + 522, hsize.y - 5   }, { 512,512 }, { 0,1 }), hsize);
			canvasMesh.MergeBatch(CanvasRect({ -hsize.x + 5,   hsize.y - 522 }, { 512,512 }, { 0,1 }), hsize);
			canvasMesh.MergeBatch(CanvasRect({ -hsize.x + 522, hsize.y - 522 }, { 512,512 }, { 0,1 }), hsize);
			renderer.Set(&canvasMesh);
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

//RUN_WINDOW(example::UICanvas)