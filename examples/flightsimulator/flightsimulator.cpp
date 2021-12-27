#include "../../include/GLRT.h"
#include "../../utils/utils.h"
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
		MaterialLib mtl;
		std::vector<Material*> m_airportMaterials;
		MeshRenderer m_TerrainMeshRenderer;
		std::vector<MeshRenderer*> m_airportMeshRenderers;
		std::vector<MeshRenderer*> m_planeRenderers;
		Shader m_TerrainShader;
		Shader m_AirportShader;
		Shader m_planeShader;
		Shader m_sm;
		Texture m_planeTexture;
		Texture m_HeightMap;
		Texture m_RGBMap;
		Texture m_Flat;
		Texture m_white;
		DirectionalLight m_light;
		float planeScale = 0.05f;
		Vector3 planePos{ -169.24,34.067,-21 };
		float planeYall = glm::pi<float>()/2;
		Color weaponcolor{ 0.5882, 0.5882, 0.5882,1.0 };
		bool viewFromLight = false;
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
			m_TerrainShader.Load(ASSETPATH("glsl/terrain.vert"), ASSETPATH("glsl/terrain.frag"));
			m_AirportShader.Load(ASSETPATH("glsl/airport.vert"), ASSETPATH("glsl/airport.frag"));
			m_planeShader.Load(ASSETPATH("glsl/plane.vert"), ASSETPATH("glsl/plane.frag"));
			m_sm.Load(ASSETPATH("glsl/shadowmapping.vert"), ASSETPATH("glsl/shadowmapping.frag"));
			m_planeTexture.Load(ASSETPATH("su.dds"));
			m_HeightMap.Load(ASSETPATH("terrain_height.png"));
			m_RGBMap.Load(ASSETPATH("terrain_rgb.png"));
			m_Flat.Load(ASSETPATH("flat.png"));
			m_white.Load(ASSETPATH("white.png"));
			LoadModel(&m_airportModel, ASSETPATH("airport.obj"));
			LoadModel(&m_planeModel, ASSETPATH("su.obj"));

		
			mtl.Add(&m_AirportShader, "ground",     [](auto& m) {m.Set("diffuse", Color{ 0.674510, 0.674510, 0.674510,1.0 }); m.renderingOrder = -1; });
			mtl.Add(&m_AirportShader, "runwayext",  [](auto& m) {m.Set("diffuse", Color{ 0.400000, 0.400000, 0.400000,1.0 }); m.renderingOrder = -1; });
			mtl.Add(&m_AirportShader, "orangeline", [](auto& m) {m.Set("diffuse", Color{ 0.800000, 0.439216, 0.203922,1.0 }); });
			mtl.Add(&m_AirportShader, "runway",     [](auto& m) {m.Set("diffuse", Color{ 0.556863, 0.556863, 0.556863,1.0 }); m.renderingOrder = -1; });
			mtl.Add(&m_AirportShader, "whiteline",  [](auto& m) {m.Set("diffuse", Color{ 1.000000, 1.000000, 1.000000,1.0 }); });
			mtl.Add(&m_AirportShader, "taxiline",   [](auto& m) {m.Set("diffuse", Color{ 1.000000, 0.972549, 0.486275,1.0 }); });

			mtl.Add(&m_planeShader, "01___Default", [&](auto& m) {m.Set("tex", m_planeTexture); m.Set("diffuse", Color{ 1.000000, 1.000000, 1.000000,1.0 }); });
			mtl.Add(&m_planeShader, "02___Default", [&](auto& m) {m.Set("tex", m_white);        m.Set("diffuse", weaponcolor); });
			m_airportMeshRenderers = MeshRenderer::CreateRenderers(m_airportModel.mergedMesh, &mtl);
			m_planeRenderers = MeshRenderer::CreateRenderers(m_planeModel.mergedMesh, &mtl);

			m_light.InitLightMap(8192, 8192);
			
		}


		virtual void Render() override
		{
			Empty3D::Render();
			Matrix4x4 airpotrWorld = glm::translate(Matrix4x4(1), Vector3{ -170,34,-20 });
			airpotrWorld = glm::scale(airpotrWorld, Vector3{ 0.03,0.03,0.03 });

			Matrix4x4 planeWorld = glm::translate(Matrix4x4(1), planePos);
			planeWorld = glm::rotate(planeWorld, planeYall, Vector3{ 0,-1,0 });
			planeWorld = glm::scale(planeWorld, Vector3{ planeScale, planeScale, planeScale });

			m_light.SetLight(m_CamController.position + Vector3{ 0,10,0 }, Vector3{ -0.2,-1,-1 }, 10);	  
			m_light.m_ShadowMappingPass.Bind();

			m_sm.Set("lightmat", m_light.matrix);
			m_sm.Set("_mv", planeWorld);

			for (auto& r : m_planeRenderers)
				r->Draw(m_sm);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			RECT rt;
			GetClientRect(hWnd, &rt);
			glViewport(0, 0, rt.right - rt.left, rt.bottom - rt.top);


			m_TerrainShader.Use();
			m_TerrainShader.Set("cam", m_Camera.GetMatrix());
			m_TerrainShader.Set("hightmap", m_HeightMap);
			m_TerrainShader.Set("flatenmap", m_Flat);
			m_TerrainShader.Set("rgbmap", m_RGBMap);
			m_TerrainMeshRenderer.Draw();

			m_AirportShader.Use();
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(0.0f, -100.0f);
			glDepthMask(false);

			m_AirportShader.Set("_mvp", m_Camera.GetMatrix()* airpotrWorld);
			m_AirportShader.Set("_mv", airpotrWorld);
			m_AirportShader.Set("shadowmap", m_light.m_ShadowMappingPass.depthBuffer);
			m_AirportShader.Set("lightmat", m_light.matrix);
			for (auto& r : m_airportMeshRenderers)
			{
				r->Draw();
			}

			glDisable(GL_POLYGON_OFFSET_FILL);
			glDepthMask(true);

			m_planeShader.Use();
			m_planeShader.Set("_mvp", m_Camera.GetMatrix()*planeWorld);
			auto m = mtl.Get("02___Default");
			if (nullptr != m)
				m->Set("diffuse", weaponcolor);
			for (auto& r : m_planeRenderers)
				r->Draw();

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
				ImGui::ColorEdit4("Weapon Color", &weaponcolor.r);
				ImGui::End();

				ImGui::Begin("Shadow mapping");
				ImGui::Image((ImTextureID)m_light.m_ShadowMappingPass.depthBuffer.id, ImVec2{ 512,512 });
				ImGui::End();
			}
		}

		virtual void SetCameraStartupParam(CameraStartupParam& param) override
		{
			Empty3D::SetCameraStartupParam(param);
			param.projection.fovY = 3.1415926 / 6;
			param.projection.zFar = 2000;
			param.projection.zNear = 0.01;

			param.position = Vector3{ -169.257,34.701,-16.767 };
			param.direction = Camera::GetDirectionFromYallPitch(Vector2{0, -0.1});
			param.moveSpeed = 0.002f;
		}
	};
}


RUN_WINDOW(example::FlightSimulator)