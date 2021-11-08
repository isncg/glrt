#include "Light.h"

void DirectionalLight::InitLightMap(int width, int height)
{
	RT.Init(width, height, 0, true);
}

void DirectionalLight::SetLight(const Vector3& origin, const Vector3& direction, float range)
{
	matrix = glm::ortho(range, range, range, range) * glm::lookAt(origin, origin + direction, glm::vec3{0, 1, 0});
}
