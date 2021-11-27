#pragma once
#include "Renderer.h"
#include "Window.h"

enum CAMERA_PROJECTION
{
	CAMPROJ_PERSPECTIVE = 0,
	CAMPROJ_ORTHO = 1
};

class Camera
{
	friend class CameraController;
	Matrix4x4 mat_view;
	Matrix4x4 mat_proj;
	Matrix4x4 mat_cache;
	Vector2 _clipRange;
public:
	Vector2& ClipRange();
	void SetProjectionMatrix(float fovY, float aspect, float zNear, float zFar);
	void SetShaderMat4(Shader& shader, const char* name = "_cam");
	Matrix4x4& GetMatrix();
};

class CameraController
{
	virtual void CalcViewMatrix(Matrix4x4& mat_proj) = 0;
	virtual void FrameUpdate() = 0;
public:
	virtual void OnMouseMove(long dx, long dy, long x, long y) = 0;
	virtual void OnKeyboard(KEYS key, KEYACTION action) = 0;
	Camera* camera;
	CameraController& Update();
	Matrix4x4& GetCameraMatrix();
};

class CameraFirstPersonController: public CameraController
{
	int ma = 0;
	int md = 0;
	int mw = 0;
	int ms = 0;
	float yall = 0;
	float pitch = 0;
	Vector3 focus;
	Vector3 fw;
	Vector3 rt;
	virtual void FrameUpdate() override;
	virtual void CalcViewMatrix(Matrix4x4& mat_proj) override;
public:
	virtual void OnMouseMove(long dx, long dy, long x, long y) override;
	virtual void OnKeyboard(KEYS key, KEYACTION action) override;
};