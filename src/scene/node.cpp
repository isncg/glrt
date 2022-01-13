#include "../../include/scene/scene.h"
#include <imgui/imgui.h>

void MeshNode::_getname(std::string& name)
{
	name = "Mesh";
}

void MeshNode::Render()
{
	this->renderer->material->pShader->Set("world", transform.worldMatrix);
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
			c->transform.locatMatrix = glm::rotate(c->transform.locatMatrix, c->transform.rotation.z, Vector3{ 0, 0, 1 });
			c->transform.locatMatrix = glm::rotate(c->transform.locatMatrix, c->transform.rotation.y, Vector3{ 0, 1, 0 });
			c->transform.locatMatrix = glm::rotate(c->transform.locatMatrix, c->transform.rotation.x, Vector3{ 1, 0, 0 });
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
	for (auto c : children)
		c->OnEnterTree();
}

void Node::AddChild(Node* pNode)
{
	pNode->parent = this;
	children.push_back(pNode);
	pNode->OnEnterTree();
}

void NodeInspector::OnInspector(Node* pNode)
{
	if (ImGui::InputFloat3("position", &pNode->transform.position.x, "%.9f"))
		pNode->transform.isDirty = true;
	if (ImGui::InputFloat3("rotation", &pNode->transform.rotation.x, "%.9f"))
		pNode->transform.isDirty = true;
	if (ImGui::InputFloat3("scale", &pNode->transform.scale.x, "%.9f"))
		pNode->transform.isDirty = true;
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
