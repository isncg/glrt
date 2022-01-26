#include "node.h"

enum SceneAxisType
{
	SceneAxisType_None = 0,
	SceneAxisType_Blender_ZupYforward,
};

class Scene: public Node
{
	virtual void _getname(std::string& name) override;
public:
	std::list<GraphicsNode*> graphicsNodes;
	void Render();
	Scene* GetScene() override;
	virtual NodeInspector* CreateInspector() override;
	virtual void Awake() override;
	virtual void Update() override;
	void Load(std::string filename, SceneAxisType axisType= SceneAxisType_Blender_ZupYforward);
};