#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"
#include "../../include/scene/scene.h"
namespace example
{
    class Scripting :public Empty3D
    {
        Scene scene;
        virtual void OnCreate() override
        {
            Empty3D::OnCreate();
            auto node = ScriptNode::CreateLuaScriptNode("LuaBehavior");
            if (NULL == node)
                return;
            scene.AddChild(node);
            scene.Awake();
        }
    };
}
RUN_WINDOW(example::Scripting)