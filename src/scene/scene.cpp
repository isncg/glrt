#include "../../include/scene/scene.h"
#include <imgui/imgui.h>
void Scene::_getname(std::string& name)
{
	name = "scene";
}

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

class SceneInspector : public NodeInspector
{
	Node* focusNode = nullptr;
	void ShowNode(Node* pNode)
	{
		if (ImGui::TreeNode((void*)pNode->GetName().c_str(), "%s", pNode->GetName().c_str()))
		{
			if(ImGui::Button("view"))
				focusNode = pNode;
			/*if (ImGui::IsItemClicked())
			{
				focusNode = pNode;
			}*/
			for (auto c : pNode->children)
			{
				ShowNode(c);
			}
			ImGui::TreePop();
		}
	}

	virtual void OnInspector(Node* pNode) override
	{
		ImGui::Begin("Scene");
		ShowNode(pNode);
		ImGui::End();

		if (nullptr != focusNode && focusNode != pNode)
		{
			ImGui::Begin("Node info");
			auto inspector = focusNode->GetInspector();
			inspector->OnInspector(focusNode);
			ImGui::End();
		}
	}
};


NodeInspector* Scene::CreateInspector()
{
	return new SceneInspector;
}
