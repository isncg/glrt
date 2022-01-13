#include "../../include/scene/scene.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
using namespace ImGui;
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

	bool TreeNode(Node* pNode, bool* is_clicked_on_label)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const char* label_end = g.TempBuffer + ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%s", pNode->GetName().c_str());
		if (pNode == focusNode)
		{
			ImU32 col = GetColorU32(ImGuiCol_TextHighlight);
			PushStyleColor(ImGuiCol_Text, col);
		}

		bool result = TreeNodeBehavior(window->GetID(pNode), ImGuiTreeNodeFlags_OpenOnArrowClick, g.TempBuffer, label_end,
			is_clicked_on_label);

		if (pNode == focusNode)
			PopStyleColor();
		return result;
	}

	void ShowNode(Node* pNode)
	{
		bool is_clicked_on_label = false;
		if (TreeNode(pNode, &is_clicked_on_label))
		{
			for (auto c : pNode->children)
			{
				ShowNode(c);
			}
			ImGui::TreePop();
		}
		if (is_clicked_on_label)
			focusNode = pNode;

	}

	virtual void OnInspector(Node* pNode) override
	{
		ImGui::Begin("Hierarchy");
		ShowNode(pNode);
		ImGui::End();

		if (nullptr != focusNode && focusNode != pNode)
		{
			ImGui::Begin("Inspector");
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
