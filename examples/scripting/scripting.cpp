#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"
#include "../../include/scene/scene.h"
namespace example
{
    class Scripting :public Empty3D
    {
        Scene scene;
        Node node;
        virtual void OnCreate() override
        {
            Empty3D::OnCreate();
            node.SetScript("FrameCounter");
            scene.AddChild(&node);
            scene.Awake();
        }

        virtual void Render() override
        {
            Empty3D::Render();
            scene.Update();
        }
    };
}
RUN_WINDOW(example::Scripting)