#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"
#include <imgui/imgui.h>
namespace example
{
    class Wireframe :public Empty3D
    {
        ASSETDIR("examples/teapot/assets/");
        Shader shader;
        MeshRenderer renderer;
        Model model;
        bool enableWireframe = true;
        virtual void OnCreate() override
        {
            Empty3D::OnCreate();
            model.Load(ASSETPATH("teapot.obj"));
            shader.Load(ASSETPATH("glsl/mesh.vert"), ASSETPATH("glsl/mesh.frag"));
            renderer.Set(&model.meshCollection.front());
        }

        virtual void Render() override
        {
            Empty3D::Render();
            shader.Set("camera", m_Camera.GetMatrix());
            if (enableWireframe)
            {
                GLASSERT(renderer.DrawWireframe(shader));
            }
            else
            {
                GLASSERT(renderer.Draw(shader));
            }
        }

        virtual void OnGUI() override
        {
            ImGui::Begin("Wireframe option");
            ImGui::Checkbox("Enable", &enableWireframe);
            ImGui::End();
        }
    };
}
//RUN_WINDOW(example::Wireframe)