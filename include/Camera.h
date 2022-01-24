#pragma once
#include "Renderer.h"
#include "Window.h"

enum CAMERA_PROJECTION
{
	CAMPROJ_PERSPECTIVE = 0,
	CAMPROJ_ORTHO = 1
};

struct CameraProjectionParam
{
	float fovY;
	float aspect;
	float zNear;
	float zFar;
};


class Camera
{
	friend class CameraController;
	friend class GlobalMaterial;
	Matrix4x4 mat_view;
	Matrix4x4 mat_proj;
	Matrix4x4 mat_cache;
	Vector2 _clipRange;
public:
	bool dynamicProjection = true;
	CameraProjectionParam projectionParam;
	Vector2& ClipRange();
	void SetProjectionMatrix(float fovY, float aspect, float zNear, float zFar);
	void SetProjectionMatrix(CameraProjectionParam& param);
	//void SetShaderMat4(Shader& shader, const char* name = "_cam");
	Matrix4x4& GetMatrix();
	const Matrix4x4& GetProjectionMatrix();

	static Vector2 GetYallPitchFromDirection(Vector3 direction);
	static Vector3 GetDirectionFromYallPitch(Vector2 yallpitch);
};

class CameraController
{
	virtual void CalcViewMatrix(Matrix4x4& mat_proj) = 0;
	virtual void FrameUpdate(double dt = 1.0) = 0;
public:
	virtual void OnMouseMove(long dx, long dy, long x, long y) = 0;
	virtual void OnKeyboard(KEYS key, KEYACTION action) = 0;
	Camera* camera;
	CameraController& Update(double dt = 1.0);
	Matrix4x4& GetCameraMatrix();
};

class CameraFirstPersonController: public CameraController
{
	int ma = 0;
	int md = 0;
	int mw = 0;
	int ms = 0;
	float vx = 0;
	float vy = 0;
	Vector3 fw;
	Vector3 rt;
	virtual void FrameUpdate(double dt = 1.0) override;
	virtual void CalcViewMatrix(Matrix4x4& mat_proj) override;
public:
	bool enabled = true;
	float yall = 0;
	float pitch = 0;
	Vector3 position;
	Vector3 velocity;
	const Vector3& GetForwardDirection();
	float speed = 20.0f;
	virtual void OnMouseMove(long dx, long dy, long x, long y) override;
	virtual void OnKeyboard(KEYS key, KEYACTION action) override;
	void Setup(Vector3& pos);
	void Setup(Vector3& pos, float yall, float pitch);
	void Setup(Vector3& pos, Vector3& lookat);
};