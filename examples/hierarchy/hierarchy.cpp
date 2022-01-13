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
        MeshRenderer renderer;
        Model model;
        MeshNode node1;
        MeshNode node2;
        Scene scene;
        virtual void OnCreate() override
        {
            Empty3D::OnCreate();
            model.Load(ASSETPATH("teapot.obj"));
            shader.Load(ASSETPATH("glsl/mesh.vert"), ASSETPATH("glsl/mesh.frag"));
            Material* pmat = new Material(&shader);
            renderer.Set(&model.meshCollection.front());
            renderer.material = pmat;

            MeshNode* pNode;
            pNode = new MeshNode;
            pNode->renderer = &renderer;
            scene.AddChild(pNode);
            for (int i = 0; i < 18; i++)
            {
                Node* pTransformNode = new Node;
                pTransformNode->transform.rotation.y = glm::pi<float>() * i / 9;
                scene.children.front()->AddChild(pTransformNode);

                pNode = new MeshNode;
                pNode->renderer = &renderer;
                pNode->transform.position.x = 2;
                pNode->transform.scale = Vector3{ 0.2,0.2,0.2 };
                pTransformNode->AddChild(pNode);
            }
        }

        virtual void Render() override
        {
            Empty3D::Render();
            shader.Set("camera", m_Camera.GetMatrix());
            scene.UpdateChildrenTransform(true);
            scene.Render();
        }

        virtual void OnGUI() override
        {
            scene.GetInspector()->OnInspector(&scene);
        }
    };
}
RUN_WINDOW(example::Hierarchy)