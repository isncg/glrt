#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"

namespace example
{
	class PBR : public Empty3D
	{
		ASSETDIR("examples/pbr/assets/");
		Shader* m_pshader;
		Model m_sphere;

		Texture m_basecolor;
		Texture m_metallic;
		Texture m_roughness;
		Texture m_ambientocclusion;
		Texture m_normal;

		std::vector<MeshRenderer*> m_renderers;
		void OnCreate() override
		{
			Empty3D::OnCreate();

			m_normal.			Load(ASSETPATH("1K_rusty_metal_8_normal.png"));
			m_basecolor.		Load(ASSETPATH("1K_rusty_metal_8_basecolor.png"));
			m_metallic.			Load(ASSETPATH("1K_rusty_metal_8_metallic.png"));
			m_roughness.		Load(ASSETPATH("1K_rusty_metal_8_roughness.png"));
			m_ambientocclusion.	Load(ASSETPATH("1K_rusty_metal_8_ambientocclusion.png"));

			m_pshader = ShaderLib::Instance().Load(ASSETPATH("glsl/pbr"));
			
			LoadModel(&m_sphere, ASSETPATH("sphere.obj"));
			MaterialLib::Instance().Add(m_pshader, "default", [&](Material& mat) 
				{
					mat.Set("albedoMap",			m_basecolor);
					mat.Set("normalMap",			m_normal);
					mat.Set("metallicMap",			m_metallic);
					mat.Set("roughnessMap",			m_roughness);
					mat.Set("aoMap",				m_ambientocclusion);				

					mat.Set("lightColor",			Vector3{200,200,200});
					mat.Set("ambientStrength",		0.03f);
					mat.Set("light[0].position",	Vector3{ -5,20,20 });
					mat.Set("light[1].position",	Vector3{ -5,20,20 });
					mat.Set("light[2].position",	Vector3{ 0,2,0 });
					mat.Set("light[3].position",	Vector3{ 0,2,0 });
					mat.Set("light[4].position",	Vector3{ 0,2,0 });
				});
			m_renderers = MeshRenderer::CreateRenderers(m_sphere.meshCollection);
		}


		void Render() override
		{
			Empty3D::Render();
			MaterialLib::Instance().Get("default")->Set("camPos", m_CamController.position);
			m_pshader->Set("world", Matrix4x4(1));
			for (auto r : m_renderers)
				r->Draw();
		}

		void OnGUI() override
		{
			MaterialLibInspector::Default().OnInspector();
		}
	};
}

//RUN_WINDOW(example::PBR);