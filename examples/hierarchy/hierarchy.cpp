#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"
#include "../../include/scene/scene.h"
namespace example
{
    class Hierarchy :public Empty3D
    {
        ASSETDIR("examples/hierarchy/assets/");
        Shader shader;
        Scene scene;
        virtual void OnCreate() override
        {
            Empty3D::OnCreate();
            scene.Load(ASSETPATH("teapots.dae"));
            shader.Load(ASSETPATH("glsl/mesh.vert"), ASSETPATH("glsl/mesh.frag"));
        }

        virtual void Render() override
        {
            Empty3D::Render();
            shader.Set("camera", m_Camera.GetMatrix());
            shader.Use();
            scene.UpdateChildrenTransform(true);
            scene.Render();
        }

        virtual void OnGUI() override
        {
            scene.OnInspector();
        }
    };
}
//RUN_WINDOW(example::Hierarchy)