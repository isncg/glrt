#include "../../include/scene/scene.h"

void Scene::Render()
{
	for (auto g : graphicsNodes)
	{
		g->Render();
	}
}

Scene* Scene::GetScene()
{
	return this;
}
