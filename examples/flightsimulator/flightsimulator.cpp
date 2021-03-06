#include "../../include/GLRT.h"
#include <utils/utils.h>
#include "../empty3d/empty3d.h"
#include <algorithm>
#include <imgui/imgui.h>
namespace example
{
	class FlightSimulator : public Empty3D
	{
		ASSETDIR("examples/flightsimulator/assets/");
		Mesh m_TerrainMesh;
		Model m_airportModel;
		Model m_planeModel;
		MaterialLib& mtl = MaterialLib::Instance();
		MaterialLibInspector mtlinspector;
		std::vector<Material*> m_airportMaterials;
		MeshRenderer m_TerrainMeshRenderer;
		std::vector<MeshRenderer*> m_airportMeshRenderers;
		std::vector<MeshRenderer*> m_planeRenderers;
		Shader* m_pTerrainShader;
		Shader* m_pAirportShader;
		Shader* m_pplaneShader;
		Shader* m_psm;
		Texture m_planeTexture;
		Texture m_HeightMap;
		Texture m_RGBMap;
		Texture m_Flat;
		Texture m_white;
		DirectionalLight m_light;
		float planeScale = 0.05f;
		Vector3 planePos{ -169.24f,34.067f,-21.0f };
		float planeYall = glm::pi<float>()/2;
		//ColorRGB weaponcolor{ 0.5882f, 0.5882f, 0.5882f };
		bool viewFromLight = false;
		char matnamebuffer[128]{0};
		void OnCreate() override
		{
			Empty3D::OnCreate();
			int size = 500;
			for (int x = -size; x <= size; x++)
			{
				for (int z = -size; z <= size; z++)
				{
					m_TerrainMesh.vertices.push_back(Vector3{ (float)x / (float)size, 0, (float)z / (float)size });
					m_TerrainMesh.normals.push_back(Vector3{ 0,0,0 });
				}
			}
			for (int x = 0; x < size * 2; x++)
			{
				for (int z = 0; z < size * 2; z++)
				{
					int i0 = (x + 0) * (size * 2 + 1) + (z + 0);
					int i1 = (x + 0) * (size * 2 + 1) + (z + 1);
					int i2 = (x + 1) * (size * 2 + 1) + (z + 1);
					int i3 = (x + 1) * (size * 2 + 1) + (z + 0);
					m_TerrainMesh.triangles.push_back(i0);
					m_TerrainMesh.triangles.push_back(i1);
					m_TerrainMesh.triangles.push_back(i2);

					m_TerrainMesh.triangles.push_back(i0);
					m_TerrainMesh.triangles.push_back(i2);
					m_TerrainMesh.triangles.push_back(i3);
				}
			}

			m_TerrainMeshRenderer.Set(&m_TerrainMesh);
			m_pTerrainShader = ShaderLib::Instance().Load(ASSETPATH("glsl/terrain"));
			m_pAirportShader = ShaderLib::Instance().Load(ASSETPATH("glsl/airport"));
			m_pplaneShader = ShaderLib::Instance().Load(ASSETPATH("glsl/plane"));
			//m_pplaneShader->materialTemplate->Set("diffuse", ColorRGB{ 1.000000, 1.000000, 1.000000 });
			m_psm = ShaderLib::Instance().Load(ASSETPATH("glsl/shadowmapping"));
			m_planeTexture.Load(ASSETPATH("su.dds"));
			m_HeightMap.Load(ASSETPATH("terrain_height.png"));
			m_RGBMap.Load(ASSETPATH("terrain_rgb.png"));
			m_Flat.Load(ASSETPATH("flat.png"));
			m_white.Load(ASSETPATH("white.png"));
			m_airportModel.Load(ASSETPATH("airport.obj"));
			m_planeModel.Load(ASSETPATH("su.obj"));

		
			mtl.Add(m_pAirportShader, "ground",     [](auto& m) {m.Set("diffuse", Color{ 0.674510f, 0.674510f, 0.674510f,1.0f }); m.renderingOrder = -1; });
			mtl.Add(m_pAirportShader, "runwayext",  [](auto& m) {m.Set("diffuse", Color{ 0.400000f, 0.400000f, 0.400000f,1.0f }); m.renderingOrder = -1; });
			mtl.Add(m_pAirportShader, "orangeline", [](auto& m) {m.Set("diffuse", Color{ 0.800000f, 0.439216f, 0.203922f,1.0f }); });
			mtl.Add(m_pAirportShader, "runway",     [](auto& m) {m.Set("diffuse", Color{ 0.556863f, 0.556863f, 0.556863f,1.0f }); m.renderingOrder = -1; });
			mtl.Add(m_pAirportShader, "whiteline",  [](auto& m) {m.Set("diffuse", Color{ 1.000000f, 1.000000f, 1.000000f,1.0f }); });
			mtl.Add(m_pAirportShader, "taxiline",   [](auto& m) {m.Set("diffuse", Color{ 1.000000f, 0.972549f, 0.486275f,1.0f }); });

			mtl.Add(m_pplaneShader, "01___Default", [&](auto& m) {m.Set("tex", m_planeTexture); });
			mtl.Add(m_pplaneShader, "02___Default", [&](auto& m) {m.Set("tex", m_white);        });
			m_airportMeshRenderers = MeshRenderer::CreateRenderers(m_airportModel.mergedMesh);
			m_planeRenderers = MeshRenderer::CreateRenderers(m_planeModel.mergedMesh);

			m_light.InitLightMap(8192, 8192);
			GlobalMaterial::Instance().Set("g_shadowbias", 0.0000001f);
		}


		virtual void Render() override
		{
			Empty3D::Render();
			Matrix4x4 airpotrWorld = glm::translate(Matrix4x4(1), Vector3{ -170,34,-20 });
			airpotrWorld = glm::scale(airpotrWorld, Vector3{ 0.03,0.03,0.03 });

			Matrix4x4 planeWorld = glm::translate(Matrix4x4(1), planePos);
			planeWorld = glm::rotate(planeWorld, planeYall, Vector3{ 0,-1,0 });
			planeWorld = glm::scale(planeWorld, Vector3{ planeScale, planeScale, planeScale });


			m_light.m_ShadowMappingPass.Bind();

			m_psm->Set("lightmat", m_light.matrix);
			m_psm->Set("_mv", planeWorld);

			for (auto& r : m_planeRenderers)
				r->Draw(*m_psm);

			RenderPass::BindDefault(this);

			m_pTerrainShader->Use();
			m_pTerrainShader->Set("hightmap", m_HeightMap);
			m_pTerrainShader->Set("flatenmap", m_Flat);
			m_pTerrainShader->Set("rgbmap", m_RGBMap);
			m_pTerrainShader->Set("world", Matrix4x4(1));
			m_TerrainMeshRenderer.Draw();

			m_pAirportShader->Use();
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(0.0f, -100.0f);
			glDepthMask(false);

			m_pAirportShader->Set("world", airpotrWorld);
			for (auto& r : m_airportMeshRenderers)
			{
				r->Draw();
			}

			glDisable(GL_POLYGON_OFFSET_FILL);
			glDepthMask(true);

			m_pplaneShader->Use();
			m_pplaneShader->Set("world", planeWorld);

			for (auto& r : m_planeRenderers)
				r->Draw();

		}

		virtual void AfterRender() override
		{
			Empty3D::AfterRender();
			Vector3 lightdir{ -1,-5,-2 };
			m_light.SetLight(m_CamController.position - (lightdir * 10.0f), lightdir, 2);
			//GlobalMaterial::Instance().Set("cameraview", viewFromLight ? m_light.matrix : m_Camera.GetMatrix());
			GlobalMaterial::Instance().Set("g_light", m_light.matrix);
			GlobalMaterial::Instance().Set("g_shadow", m_light.m_ShadowMappingPass.depthBuffer);

		}

		virtual void OnGUI() override
		{
			if (!m_CamController.enabled)
			{
				ImGui::Begin("Camera Info");
				ImGui::InputFloat3("Position", &m_CamController.position.x);
				ImGui::InputFloat("Yall", &m_CamController.yall);
				ImGui::InputFloat("Pitch", &m_CamController.pitch);
				ImGui::InputFloat("Speed", &m_CamController.speed);
				ImGui::Checkbox("Light view", &viewFromLight);
				ImGui::Checkbox("Projection Info", &m_Camera.dynamicProjection);
				if (m_Camera.dynamicProjection)
				{
					ImGui::DragFloat("FovY", &m_Camera.projectionParam.fovY, 0.01f, 0.01f, 3.0f);
					ImGui::InputFloat("aspect", &m_Camera.projectionParam.aspect);
					ImGui::InputFloat("zNear", &m_Camera.projectionParam.zNear);
					ImGui::InputFloat("zFar", &m_Camera.projectionParam.zFar);
				}
				ImGui::End();

				ImGui::Begin("Plane Info");
				ImGui::DragFloat3("Position", &planePos.x, 0.01f);
				ImGui::DragFloat("Scale", &planeScale, 0.01f);
				ImGui::DragFloat("Yall", &planeYall, 0.01f);
				ImGui::End();

				ImGui::Begin("Shadow mapping");
				ImGui::Image((ImTextureID)m_light.m_ShadowMappingPass.depthBuffer.id, ImVec2{ 512,512 });
				ImGui::End();

				mtlinspector.OnInspector();
				GlobalMaterial::Instance().OnInspector();
			}
		}

		virtual void SetCameraStartupParam(CameraStartupParam& param) override
		{
			Empty3D::SetCameraStartupParam(param);
			param.projection.fovY = 3.1415926f / 6;
			param.projection.zFar = 2000.0f;
			param.projection.zNear = 0.01f;

			param.position = Vector3{ -169.257,34.701,-16.767 };
			param.direction = Camera::GetDirectionFromYallPitch(Vector2{0, -0.1});
			param.moveSpeed = 0.002f;
		}
	};
}


//RUN_WINDOW(example::FlightSimulator)