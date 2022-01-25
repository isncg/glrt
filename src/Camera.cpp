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
	projectionParam.zNear = zNear;
	projectionParam.zFar = zFar;
	projectionParam.fovY = fovY;
	projectionParam.aspect = aspect;
}

void Camera::SetProjectionMatrix(CameraProjectionParam& param)
{
	SetProjectionMatrix(param.fovY, param.aspect, param.zNear, param.zFar);
}

//void Camera::SetShaderMat4(Shader& shader, const char* name)
//{
//	shader.Set(name, mat_proj * mat_view);
//}

Matrix4x4& Camera::GetMatrix()
{
	if (dynamicProjection)
	{
		_clipRange.x = projectionParam.zNear;
		_clipRange.y = projectionParam.zFar;
		mat_proj = Matrix4x4::Perspective(projectionParam.fovY, projectionParam.aspect, projectionParam.zNear, projectionParam.zFar);
	}
	mat_cache = mat_proj * mat_view;
	return mat_cache;
}

const Matrix4x4& Camera::GetProjectionMatrix()
{
	return mat_proj;
}

Vector2 Camera::GetYallPitchFromDirection(Vector3 direction)
{
	return Vector2
	{
		glm::atan(direction.z, direction.x) + glm::pi<float>() / 2,
		glm::atan(direction.y, glm::length(Vector2{ direction.x, direction.z }))
	};
}

Vector3 Camera::GetDirectionFromYallPitch(Vector2 yallpitch)
{
	return Vector3
	{
		glm::sin(yallpitch.x) * glm::cos(yallpitch.y),
		glm::sin(yallpitch.y),
	   -glm::cos(yallpitch.x) * glm::cos(yallpitch.y)
	};
}


CameraController& CameraController::Update(double dt)
{
	FrameUpdate(dt);
	CalcViewMatrix(camera->mat_view);
	return *this;
}

Matrix4x4& CameraController::GetCameraMatrix()
{
	return camera->GetMatrix();
}

const Vector3& CameraFirstPersonController::GetForwardDirection()
{
	return fw;
}

void CameraFirstPersonController::OnMouseMove(long dx, long dy, long x, long y)
{
	if (!enabled)
		return;
#define M_PI 3.14159265358979323846f
	float hp = M_PI / 2.0f - 0.000001f;
	yall += dx * 0.005f;
	pitch -= dy * 0.005f;
	if (pitch > hp)
		pitch = hp;
	if (pitch < -hp)
		pitch = -hp;
}

void CameraFirstPersonController::OnKeyboard(KEYS key, KEYACTION action)
{
	if (key == KEYS::KEY_ESCAPE && action == KEYACTION::KEYDOWN)
	{
		enabled = !enabled;
	}
	if (!enabled)
		return;
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
	this->position = pos;
}

void CameraFirstPersonController::Setup(Vector3& pos, float yall, float pitch)
{
	this->position = pos;
	this->yall = yall;
	this->pitch = pitch;
}


void CameraFirstPersonController::FrameUpdate(double dt)
{
	if (!enabled)
		return;
	//Vector3 r{ cos(yall) * cos(pitch), sin(pitch), sin(yall) * cos(pitch) };
	fw.x = sin(yall) * cos(pitch);
	fw.y = sin(pitch);
	fw.z = -cos(yall) * cos(pitch);
	Vector3 rt{ cos(yall), 0, sin(yall) };
	Vector3 velocity = ((float)(md - ma) * rt + (float)(mw - ms) * fw) * speed;
	float w = pow(0.5, dt / 0.1);
	this->velocity = w * this->velocity + (1.0f - w) * velocity;
	position += this->velocity * (float)dt;
}

void CameraFirstPersonController::CalcViewMatrix(Matrix4x4& mat_proj)
{
	mat_proj = Matrix4x4::LookAt(position, position + fw, Vector3{ 0,1,0 });
}

