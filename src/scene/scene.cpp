#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "../../include/scene/scene.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
using namespace ImGui;
#include "../../utils/utils.h"
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

void AwakeChildren(Node* pNode)
{
	for (auto pchild : pNode->children)
	{
		pchild->Awake();
		AwakeChildren(pchild);
	}
}

void Scene::Awake()
{
	isActive = true;
	AwakeChildren(this);
}

void UpdateChildren(Node* pNode)
{
	for (auto pchild : pNode->children)
	{
		pchild->Update();
		UpdateChildren(pchild);
	}
}


void Scene::Update()
{
	UpdateChildren(this);
}

extern bool LoadMesh(Mesh* output, aiMesh* input);
bool LoadSceneNode(Node* pOut, aiNode* pIn, const aiScene* pScene, SceneAxisType axisType)
{
	//pIn->mTransformation
	//TODO: Decomposit TRS matrix;
	// Not sure whether I should use row or column vector for aiNode
	//https://answers.unity.com/questions/402280/how-to-decompose-a-trs-matrix.html
	//https://learnopencv.com/rotation-matrix-to-euler-angles/
	Vector3 right = { pIn->mTransformation.a1, pIn->mTransformation.b1,pIn->mTransformation.c1 };
	Vector3 up = { pIn->mTransformation.a2, pIn->mTransformation.b2,pIn->mTransformation.c2 };
	Vector3 forward = { pIn->mTransformation.a3, pIn->mTransformation.b3,pIn->mTransformation.c3 };
	Vector3 pos = { pIn->mTransformation.a4, pIn->mTransformation.b4,pIn->mTransformation.c4 };
	Vector3 scale = { glm::length(right), glm::length(up), glm::length(forward) };
	right /= scale.x;
	up /= scale.y;
	forward /= scale.z;
	auto eulerAngles = glm::eulerAngles(glm::qua<float>(glm::mat3(right.x, up.x, forward.x, right.y, up.y, forward.y, right.z, up.z, forward.z)));
	switch (axisType)
	{
	case SceneAxisType_Blender_ZupYforward:
		eulerAngles *= -1;
		break;
	}
	pOut->transform.position = pos;
	pOut->transform.scale = scale;
	pOut->transform.rotation = eulerAngles;

	for (int i = 0; i < pIn->mNumMeshes; i++)
	{
		MeshNode* pNode = new MeshNode();
		Mesh mesh;
		LoadMesh(&mesh, pScene->mMeshes[pIn->mMeshes[i]]);
		pNode->renderer = new MeshRenderer();
		pNode->renderer->Set(&mesh);
		pOut->AddChild(pNode);
	}

	for (int i = 0; i < pIn->mNumChildren; i++)
	{
		Node* pNode = new Node();
		pOut->AddChild(pNode);
		LoadSceneNode(pNode, pIn->mChildren[i], pScene, axisType);
	}
	return true;
}


void Scene::Load(std::string filename, SceneAxisType axisType)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_CalcTangentSpace |
		//aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!scene)
	{
		log(importer.GetErrorString(), LogLevel::Error);
		return;
	}
	LoadSceneNode(this, scene->mRootNode, scene, axisType);
	switch (axisType)
	{
	case SceneAxisType_Blender_ZupYforward:
		transform.worldMatrix = glm::rotate(Matrix4x4(1), -glm::pi<float>() / 2, Vector3{ 1,0,0 });
		break;
	default:
		break;
	}

	Awake();
}
