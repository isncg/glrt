#include "../../include/GLRT.h"
#include "../../utils/utils.h"

namespace example
{
	class Teapot :public Window
	{
	protected:
		Shader shader;
		MeshRenderer renderer;
		Model model;
		int frame = 0;
		Matrix4x4 mat = Matrix4x4(1);
		long width, height;
		virtual void OnCreate() override
		{
			Window::OnCreate();
			LoadModel(&model, GetAssetPath("teapot.obj"));
			shader.Load(GetAssetPath("glsl/mesh.vert"), GetAssetPath("glsl/mesh.frag"));
			shader.Use();

			renderer.Set(&model.meshCollection.front());

			GetInitSize(&width, &height);
			glEnable(GL_DEPTH_TEST);
			glAssert("oncreate finish");
		}

		virtual void OnResize(long width, long height) override { this->width = width; this->height = height; }

		virtual void Render() override
		{
			frame++;
			mat =
				Matrix4x4::Perspective(3.1415926 / 3, (float)width / (float)height, 0.1, 100) *
				Matrix4x4::LookAt(Vector3{ 3 * cos(0.005 * frame),2, 3 * sin(0.005 * frame) }, Vector3{ 0,0.5,0 }, Vector3{ 0,1,0 });
			glClearColor(0.2, 0.2, 0.2, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader.Use();
			shader.Set("_mvp", mat);
			GLASSERT(renderer.Draw());
		}
	};
}
//RUN_WINDOWEX(example::Teapot, "examples/teapot/assets/")


namespace example
{
	class TeapotWithMouseLook : public Teapot
	{
#define M_PI 3.14159265358979323846
		float yall = 0;
		float pitch = 0;
		float hp = M_PI / 2.0f - 0.000001f;
		virtual void OnMouseMove(long dx, long dy, long x, long y) override
		{
			yall += dx * 0.005f;
			pitch += dy * 0.005;
			if (pitch > hp)
				pitch = hp;
			if (pitch < -hp)
				pitch = -hp;
		}

		virtual void Render() override
		{
			mat =
				Matrix4x4::Perspective(3.1415926 / 2, 1.333f, 0.1, 100) *
				Matrix4x4::LookAt(Vector3{ 5 * cos(yall) * cos(pitch),5 * sin(pitch), 5 * sin(yall) * cos(pitch) }, Vector3{ 0,0,0 }, Vector3{ 0,1,0 });
			glClearColor(0.2, 0.2, 0.2, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader.Use();
			shader.Set("_mvp", mat);
			GLASSERT(renderer.Draw());
		}
	};
}

RUN_WINDOWEX(example::TeapotWithMouseLook, "examples/teapot/assets/")