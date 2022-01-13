#include <glad/glad.h>
#include <glm/glm.hpp>
#include <list>
#include "../GLRT.h"
class Node;
class Transform;

class Transform
{
public:
	bool isDirty = true;
	Matrix4x4 locatMatrix = Matrix4x4(1);
	Matrix4x4 worldMatrix = Matrix4x4(1);
	Vector3 position = Vector3(0);
	Vector3 rotation = Vector3(0);
	Vector3 scale = Vector3(1);
};

class NodeInspector
{
public:
	bool expandTransform = true;
	virtual void OnInspector(Node* pNode);
};

class Scene;
class Node
{
	std::string _name;
	virtual void _getname(std::string& name);
	NodeInspector* inspector = nullptr;
public:
	std::string& GetName();
	Transform transform;
	Node* parent = nullptr;
	std::list<Node*> children;
	void UpdateChildrenTransform(bool force);
	NodeInspector* GetInspector();
	virtual NodeInspector* CreateInspector();
	virtual Scene* GetScene();
	virtual void OnEnterTree();
	void AddChild(Node* pNode);
};

class GraphicsNode : public Node
{
	virtual void _getname(std::string& name) override;
public:
	virtual void Render() = 0;
	virtual void OnEnterTree() override;
};

class MeshNode :public GraphicsNode
{
	virtual void _getname(std::string& name) override;
public:
	Mesh* mesh;
	MeshRenderer* renderer;
	virtual void Render() override;
	virtual NodeInspector* CreateInspector() override;
};


class CameraNode : public Node
{
	virtual void _getname(std::string& name) override;
public:
	Camera camera;
};