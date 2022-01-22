#include "node.h"
class Scene: public Node
{
	virtual void _getname(std::string& name) override;
public:
	std::list<GraphicsNode*> graphicsNodes;
	void Render();
	Scene* GetScene() override;
	virtual NodeInspector* CreateInspector() override;
	void Load(std::string filename);
};