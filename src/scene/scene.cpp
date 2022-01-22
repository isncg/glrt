#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "../../include/scene/scene.h"
#include <imgui/imgui.h>
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

extern bool LoadMesh(Mesh* output, aiMesh* input);
bool LoadSceneNode(Node* pOut, aiNode* pIn, const aiScene* pScene)
{
	//pIn->mTransformation
	//TODO: Decomposit TRS matrix;
	// Not sure whether I should use row or column vector for aiNode
	//https://answers.unity.com/questions/402280/how-to-decompose-a-trs-matrix.html
	//https://learnopencv.com/rotation-matrix-to-euler-angles/
	Vector3 right =    { pIn->mTransformation.a1, pIn->mTransformation.b1,pIn->mTransformation.c1 };
	Vector3 up =       { pIn->mTransformation.a2, pIn->mTransformation.b2,pIn->mTransformation.c2 };
	Vector3 forward =  { pIn->mTransformation.a3, pIn->mTransformation.b3,pIn->mTransformation.c3 };
	Vector3 pos =      { pIn->mTransformation.a4, pIn->mTransformation.b4,pIn->mTransformation.c4 };
	Vector3 scale = { glm::length(right), glm::length(up), glm::length(forward) };
	right /= scale.x;
	up /= scale.y;
	forward /= scale.z;
	auto eularAngles = glm::eulerAngles(glm::qua<float>(glm::mat3(right.x, up.x, forward.x, right.y, up.y, forward.y, right.z, up.z, forward.z)));
	eularAngles.z *= -1;
	pOut->transform.position = pos;
	pOut->transform.scale = scale;
	pOut->transform.rotation = eularAngles;

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
		LoadSceneNode(pNode, pIn->mChildren[i], pScene);
	}
	return true;
}


void Scene::Load(std::string filename)
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
	LoadSceneNode(this, scene->mRootNode, scene);
}
