#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"
#include <imgui/imgui.h>
namespace example
{
    class Wireframe :public Empty3D
    {
        ASSETDIR("examples/wireframe/assets/");
        Shader shader;
        std::vector<MeshRenderer*> renderers;
        Model model;
        bool enableWireframe = true;
        virtual void OnCreate() override
        {
            Empty3D::OnCreate();
            model.Load(ASSETPATH("cubes.obj"));
            shader.Load(ASSETPATH("glsl/mesh.vert"), ASSETPATH("glsl/mesh.frag"));
            renderers = MeshRenderer::CreateRenderers(model.meshCollection);
        }

        virtual void Render() override
        {
            Empty3D::Render();
            glLineWidth(3.0f);
            shader.Set("camera", m_Camera.GetMatrix());
            if (enableWireframe)
            {
                for (auto r : renderers)
                    r->DrawWireframe(shader);
            }
            else
            {
                for (auto r : renderers)
                    r->Draw(shader);
            }
            glLineWidth(1.0f);
        }

        virtual void OnGUI() override
        {
            ImGui::Begin("Wireframe option");
            ImGui::Checkbox("Enable", &enableWireframe);
            ImGui::End();
        }

        virtual void SetCameraStartupParam(CameraStartupParam& param) override
        {
            Empty3D::SetCameraStartupParam(param);
            param.moveSpeed = 0.01f;
        }
    };
}
RUN_WINDOW(example::Wireframe)