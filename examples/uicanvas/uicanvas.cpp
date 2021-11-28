#include "../../include/GLRT.h"
#include "../../utils/utils.h"

namespace example
{
	class UICanvas :public Window
	{
		Shader shader;
		CanvasMesh canvasMesh;
		Texture texture;
		CanvasRenderer renderer;
		virtual void OnCreate() override
		{
			Window::OnCreate();
			LoadTexture(&texture, GetAssetPath("lena.jpg"));
			shader.Load(GetAssetPath("glsl/canvas.vert"), GetAssetPath("glsl/canvas.frag"));
			shader.Set("tex", texture);
			shader.Use();
			auto hsize = GetClientSize()*0.5f;
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

RUN_WINDOWEX(example::UICanvas, "examples/uicanvas/assets/")