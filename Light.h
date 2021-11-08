#pragma once
#include "Renderer.h"
#include "RenderTarget.h"

class Light
{
};

class DirectionalLight: public Light
{
public:
	Matrix4x4 matrix;
	RenderTarget RT;
	void InitLightMap(int width, int height);
	void SetLight(const Vector3& origin, const Vector3& direction, float range);
};

