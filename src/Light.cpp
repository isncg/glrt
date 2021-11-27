#include "../include/Light.h"

void DirectionalLight::InitLightMap(int width, int height)
{
	m_ShadowMappingPass.Init(width, height, 0, true);
}

void DirectionalLight::SetLight(const Vector3& origin, const Vector3& direction, float range)
{
	matrix = glm::orthoNO(-range, range, -range, range, 1.0f, 5000.0f) * glm::lookAt(origin, origin + direction, glm::vec3{ 0, 1, 0 });
}