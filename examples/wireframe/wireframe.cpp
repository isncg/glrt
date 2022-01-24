#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"
#include <imgui/imgui.h>
namespace example
{
    class Wireframe :public Empty3D
    {
        ASSETDIR("examples/wireframe/assets/");
        Shader wireframeShader;
        Shader depthShader;
        std::vector<MeshRenderer*> renderers;
        Model model;
        bool enableWireframe = true;
        RenderPass depthPass;
        virtual void OnCreate() override
        {
            Empty3D::OnCreate();
            model.Load(ASSETPATH("cubes.obj"));
            wireframeShader.Load(ASSETPATH("glsl/wire.vert"), ASSETPATH("glsl/wire.frag"));
            depthShader.Load(ASSETPATH("glsl/depth.vert"), ASSETPATH("glsl/depth.frag"));
            renderers = MeshRenderer::CreateRenderers(model.meshCollection);
            depthPass.Init(1024, 1024, 0, true);
            wireframeShader.Set("depthbuf", depthPass.depthBuffer);
        }

        virtual void Render() override
        {
            // depth pass
            wireframeShader.Set("camera", m_Camera.GetMatrix());
            depthShader.Set("camera", m_Camera.GetMatrix());
            depthPass.Bind();
            for (auto r : renderers)
                r->Draw(depthShader);

            // wireframe pass
            RenderPass::BindDefault(this);
            Empty3D::Render();
            glLineWidth(3.0f);
            if (enableWireframe)
            {
                for (auto r : renderers)
                    r->DrawWireframe(wireframeShader);
            }
            else
            {
                for (auto r : renderers)
                    r->Draw(wireframeShader);
            }
            glLineWidth(1.0f);
        }

        virtual void OnGUI() override
        {
            ImGui::Begin("Wireframe option");
            ImGui::Checkbox("Enable", &enableWireframe);
            ImGui::End();
        }
    };
}
RUN_WINDOW(example::Wireframe)