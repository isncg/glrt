#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"
#include "../../include/scene/scene.h"
#include "../../include/Script.h"
namespace example
{
    class Scripting :public Empty3D
    {
        ASSETDIR("examples/scripting/assets/");
        Scene scene;
        Node node;
        virtual void OnCreate() override
        {
            Empty3D::OnCreate();
            LoadScriptFile(ASSETPATH("scripts/script.lua"));
            SetScript("ScriptingWindow");
            InvokeScript("onCreate");
            //node.SetScript("FrameCounter");
            scene.AddChild(&node);
            scene.Awake();
        }

        virtual void Render() override
        {
            Empty3D::Render();
            InvokeScript("render");
            scene.Update();
        }
    };
}
RUN_WINDOW(example::Scripting)