#include "framework.h"
#include "Application.h"
#include "Window.h"
#include "Renderer.h"
#include "utils/glhelpers.h"
#include "utils/asset_loader.h"
#include <math.h>
class Sample_ModelLoadingAndRendering :public Window
{
	Shader shader;
	MeshRenderer renderer;
	Model model;
	int frame = 0;
	Matrix4x4 mat = Matrix4x4(1);
	virtual void OnCreate() override
	{
		Window::OnCreate();
		LoadModel(&model, "assets/teapot.obj");
		shader.Load("glsl/mesh.vert", "glsl/mesh.frag");
		shader.Use();

		renderer.Set(&model.meshCollection.front());
		glEnable(GL_DEPTH_TEST);
		glAssert("oncreate finish");
	}

	virtual void Render() override
	{
		frame++;
		mat =
			Matrix4x4::Perspective(3.1415926/2, 1.333f, 0.1, 100) *
			Matrix4x4::LookAt(Vector3{ 5*cos(0.005*frame),3, 5*sin(0.005*frame) }, Vector3{ 0,0,0 }, Vector3{ 0,1,0 });
		glClearColor(0.2, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.Use();
		shader.Set("_mvp", mat);
		GLASSERT(renderer.Draw());
	}
};

//RUN_WINDOW(Sample_ModelLoadingAndRendering)

class Sample_UICanvas :public Window
{
	Shader shader;
	CanvasMesh canvasMesh;
	CanvasRect cr;
	Texture texture;
	CanvasRenderer renderer;
	virtual void OnCreate() override
	{
		Window::OnCreate();
		LoadTexture(&texture, "assets/lena.jpg");
		RECT rect;
		GetClientRect(hWnd, &rect);
		float hw = (rect.right - rect.left) / 2.0f;
		float hh = (rect.bottom - rect.top) / 2.0f;
		cr.pos.x = -hw;
		cr.pos.y = hh;
		cr.xMin = 5;
		cr.yMax = -5;
		cr.xMax = 405;
		cr.yMin = -405;
		canvasMesh.Set(cr, hw, hh);
		
		shader.Load("glsl/canvas.vert", "glsl/canvas.frag");
		shader.Set("tex", &texture);
		shader.Use();

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

//RUN_WINDOW(Sample_UICanvas)


class Sample_MouseLook :public Window
{
#define M_PI 3.14159265358979323846
	Shader shader;
	MeshRenderer renderer;
	Model model;
	float yall = 0;
	float pitch = 0;
	Matrix4x4 mat;
	float hp = M_PI / 2.0f - 0.000001f;
	virtual void OnMouseMove(long dx, long dy, long x, long y) override
	{
		yall += dx*0.005f;
		pitch += dy * 0.005;
		if (pitch > hp)
			pitch = hp;
		if (pitch < -hp)
			pitch = -hp;
	}

	virtual void OnCreate() override
	{
		Window::OnCreate();
		LoadModel(&model, "assets/teapot.obj");
		shader.Load("glsl/mesh.vert", "glsl/mesh.frag");
		shader.Use();

		renderer.Set(&model.meshCollection.front());
		glEnable(GL_DEPTH_TEST);
		glAssert("oncreate finish");
	}

	virtual void Render() override
	{
		mat =
			Matrix4x4::Perspective(3.1415926 / 2, 1.333f, 0.1, 100) *
			Matrix4x4::LookAt(Vector3{ 5 * cos(yall)*cos(pitch),5*sin(pitch), 5 * sin(yall) * cos(pitch) }, Vector3{ 0,0,0 }, Vector3{ 0,1,0 });
		glClearColor(0.2, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.Use();
		shader.Set("_mvp", mat);
		GLASSERT(renderer.Draw());
	}
};
//RUN_WINDOW(Sample_MouseLook)
#include "Camera.h"
#include "RenderTarget.h"
#include "Light.h"
#include "Material.h"
class Sample_BSPViewer :public Window
{
	Camera firstPersonCamera;
	CameraFirstPersonController firstPersonCamController;

	Model bspMapModel;
	Texture bspMapTexture;
	Shader meshShader;
	Material meshMaterial;
	MeshRenderer meshRenderer;
	
	RenderTarget forwardRT;
	Shader canvasShader;
	Material canvasMaterial;
	CanvasRenderer canvasRenderer;

	DirectionalLight light;
	Shader lightmapMeshShader;
	Material lightmapMaterial;

	virtual void OnMouseMove(long dx, long dy, long x, long y) override
	{
		firstPersonCamController.OnMouseMove(dx, dy, x, y);
	}

	virtual void OnKeyboard(KEYS key, KEYACTION action) override
	{
		firstPersonCamController.OnKeyboard(key, action);
	}

	virtual void OnCreate() override
	{
		Window::OnCreate();
		light.InitLightMap(8192, 8192);
		Vector3 lightPos{ -2000,2000,-2000 };
		Vector3 lightDir{ 1,-1,1 };
		float lightRange = 4096;
		light.SetLight(lightPos, lightDir, lightRange);
		lightmapMeshShader.Load("glsl/mesh_depth.vert", "glsl/mesh_depth.frag");
		lightmapMaterial.Set(lightmapMeshShader);
		lightmapMaterial.Set("lightmat", light.matrix);


		firstPersonCamera.SetProjectionMatrix(3.1415926 / 2, 1.333f, 1, 5000);
		firstPersonCamController.camera = &firstPersonCamera;
		LoadBSPMap(&bspMapModel, "assets/de_dust2.bsp");
		LoadTexture(&bspMapTexture, "assets/256.bmp");
		meshShader.Load("glsl/mesh_shadowmap.vert", "glsl/mesh_shadowmap.frag");
		meshMaterial.Set(meshShader);
		meshMaterial.Set("tex", bspMapTexture);
		meshMaterial.Set("shadowmap", light.RT.depthTexture);
		meshMaterial.Set("lightmat", light.matrix);
		
		forwardRT.Init(4096, 2048, 1, true);

		meshRenderer.Set(&bspMapModel.meshCollection.front());

		canvasRenderer.SetFullScreen();
		canvasShader.Load("glsl/canvas.vert", "glsl/canvas.frag");
		canvasMaterial.Set(canvasShader);
		canvasMaterial.Set("ssize",16);
		canvasMaterial.Set("blur", 4);
		canvasMaterial.Set("clipRange", firstPersonCamera.ClipRange());
		canvasMaterial.Set("tex", forwardRT.colorTextures[0]);
		canvasMaterial.Set("depth", light.RT.depthTexture);

		glEnable(GL_DEPTH_TEST);
	}

	virtual void BeforeRender() override
	{
		light.RT.Bind();
		GLASSERT(glClearColor(0.2, 0.2, 0.2, 1));
		GLASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		lightmapMaterial.Use();
		meshRenderer.Draw();

		forwardRT.Bind();
		firstPersonCamController.Update();
		GLASSERT(glClearColor(0.2, 0.2, 0.2, 1));
		GLASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		meshMaterial.Use();
		firstPersonCamera.SetShaderMat4(meshShader, "_mvp");
		GLASSERT(meshRenderer.Draw());
	}

	virtual void Render() override
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		canvasMaterial.Use();
		canvasRenderer.Draw();
	}
};
RUN_WINDOW(Sample_BSPViewer)