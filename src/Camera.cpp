#include "../include/Camera.h"

Vector2& Camera::ClipRange()
{
	return _clipRange;
}

void Camera::SetProjectionMatrix(float fovY, float aspect, float zNear, float zFar)
{
	_clipRange.x = zNear;
	_clipRange.y = zFar;
	mat_proj = Matrix4x4::Perspective(fovY, aspect, zNear, zFar);
}

void Camera::SetShaderMat4(Shader& shader, const char* name)
{
	shader.Set(name, mat_proj * mat_view);
}

Matrix4x4& Camera::GetMatrix()
{
	mat_cache = mat_proj * mat_view;
	return mat_cache;
}


CameraController& CameraController::Update()
{
	FrameUpdate();
	CalcViewMatrix(camera->mat_view);
	return *this;
}

Matrix4x4& CameraController::GetCameraMatrix()
{
	return camera->GetMatrix();
}

void CameraFirstPersonController::OnMouseMove(long dx, long dy, long x, long y)
{
#define M_PI 3.14159265358979323846
	float hp = M_PI / 2.0f - 0.000001f;
	yall += dx * 0.005f;
	pitch -= dy * 0.005;
	if (pitch > hp)
		pitch = hp;
	if (pitch < -hp)
		pitch = -hp;
}

void CameraFirstPersonController::OnKeyboard(KEYS key, KEYACTION action)
{
	if (key == KEYS::KEY_W)
	{
		mw = action == KEYACTION::KEYDOWN ? 1 : 0;
	}

	if (key == KEYS::KEY_A)
	{
		ma = action == KEYACTION::KEYDOWN ? 1 : 0;
	}

	if (key == KEYS::KEY_S)
	{
		ms = action == KEYACTION::KEYDOWN ? 1 : 0;
	}

	if (key == KEYS::KEY_D)
	{
		md = action == KEYACTION::KEYDOWN ? 1 : 0;
	}
}

void CameraFirstPersonController::Setup(Vector3& pos)
{
	this->focus = pos;
}

void CameraFirstPersonController::Setup(Vector3& pos, float yall, float pitch)
{
	this->focus = pos;
	this->yall = yall;
	this->pitch = pitch;
}


void CameraFirstPersonController::FrameUpdate()
{
	//Vector3 r{ cos(yall) * cos(pitch), sin(pitch), sin(yall) * cos(pitch) };
	fw.x = cos(yall) * cos(pitch);
	fw.y = sin(pitch);
	fw.z = sin(yall) * cos(pitch);
	Vector3 rt{ sin(yall), 0, -cos(yall) };
	focus += -(float)(md - ma) * rt * speed;
	focus += (float)(mw - ms) * fw * speed;
}

void CameraFirstPersonController::CalcViewMatrix(Matrix4x4& mat_proj)
{
	mat_proj = Matrix4x4::LookAt(focus, focus + fw, Vector3{ 0,1,0 });
}

