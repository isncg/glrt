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
            node1.renderer = &renderer;
            node1.transform.position = Vector3{ 0,0,0 };
            node2.renderer = &renderer;
            node2.transform.position = Vector3{ 4,0,0 };
			node2.transform.scale = Vector3{ 0.2,0.2,0.2 };
            scene.AddChild(&node1);
            node1.AddChild(&node2);
        }

        virtual void Render() override
        {
            Empty3D::Render();
            shader.Set("camera", m_Camera.GetMatrix());
            scene.UpdateChildrenTransform(true);
            scene.Render();
        }
    };
}
//RUN_WINDOW(example::Hierarchy)