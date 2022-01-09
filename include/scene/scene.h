#include "node.h"
class Scene: public Node
{
public:
	std::list<GraphicsNode*> graphicsNodes;
	void Render();
	Scene* GetScene() override;
};