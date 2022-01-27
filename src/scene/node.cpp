#include "../../include/scene/scene.h"
#include <imgui/imgui.h>
#include <glm/gtx/euler_angles.hpp>
void MeshNode::_getname(std::string& name)
{
	name = "Mesh";
}

void MeshNode::Render()
{
	auto shader = Shader::Current();
	if (this->renderer != NULL && this->renderer->material != NULL && this->renderer->material->pShader != NULL)
		shader = this->renderer->material->pShader;
	shader->Set("world", transform.worldMatrix);
	this->renderer->Draw();
}

class MeshNodeInspector: public NodeInspector
{

};


NodeInspector* MeshNode::CreateInspector()
{
	return new MeshNodeInspector;
}

void Node::_getname(std::string& name)
{
	name = "Node";
}

std::string& Node::GetName()
{
	if (_name.size() == 0)
		_getname(_name);
	return _name;
}

void Node::UpdateChildrenTransform(bool force)
{
	for (auto c : children)
	{
		bool needUpdate = c->transform.isDirty || force;
		if (needUpdate)
		{
			c->transform.locatMatrix = Matrix4x4(1);
			c->transform.locatMatrix = glm::translate(c->transform.locatMatrix, c->transform.position);
			c->transform.locatMatrix *= glm::eulerAngleYXZ(c->transform.rotation.y, c->transform.rotation.x, c->transform.rotation.z);
			c->transform.locatMatrix = glm::scale(c->transform.locatMatrix, c->transform.scale);
			c->transform.worldMatrix = transform.worldMatrix * c->transform.locatMatrix;
		}
		c->UpdateChildrenTransform(needUpdate);
	}
}

NodeInspector* Node::GetInspector()
{
	if (nullptr == inspector)
		inspector = CreateInspector();
	if (nullptr == inspector)
		inspector = new NodeInspector;
	return inspector;
}

void Node::OnInspector()
{
	GetInspector()->OnInspector(this);
}

NodeInspector* Node::CreateInspector()
{
	return new NodeInspector();
}

Scene* Node::GetScene()
{
	if(nullptr == parent)
		return nullptr;
	return parent->GetScene();
}

void Node::OnEnterTree()
{
}

void Node::Awake()
{
}

void Node::Update()
{
}

void NodeEnterTree(Node* pNode, bool needAwake)
{
	pNode->OnEnterTree();
	if (pNode->pScriptContext)
		pNode->pScriptContext->Invoke("on_enter_tree");
	for (auto c : pNode->children)
	{
		NodeEnterTree(c, needAwake);
	}
	if (needAwake)
	{
		pNode->Awake();
		if (pNode->pScriptContext)
			pNode->pScriptContext->Invoke("awake");
	}
}


void Node::AddChild(Node* pNode)
{
	pNode->parent = this;
	children.push_back(pNode);
	auto scene = GetScene();
	NodeEnterTree(pNode, scene && scene->isActive);
}

void NodeInspector::OnInspector(Node* pNode)
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		if (ImGui::InputFloat3("position", &pNode->transform.position.x, "%.6f"))
			pNode->transform.isDirty = true;
		if (ImGui::InputFloat3("rotation", &pNode->transform.rotation.x, "%.6f"))
			pNode->transform.isDirty = true;
		if (ImGui::InputFloat3("scale", &pNode->transform.scale.x, "%.6f"))
			pNode->transform.isDirty = true;
	}
}

void GraphicsNode::_getname(std::string& name)
{
	name = "Graphics";
}

void GraphicsNode::OnEnterTree()
{
	Node::OnEnterTree();
	GetScene()->graphicsNodes.push_back(this);
}

void CameraNode::_getname(std::string& name)
{
	name = "Camera";
}