#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"

namespace example
{
	class FlightSimulator : public Empty3D
	{
		ASSETDIR("examples/flightsimulator/assets/");
		Mesh m_TerrainMesh;
		Model m_airportModel;
		MeshRenderer m_TerrainMeshRenderer;
		std::vector<MeshRenderer> m_airportMeshRenderers;
		Shader m_TerrainShader;
		Shader m_AirportShader;
		Texture m_HeightMap;
		Texture m_RGBMap;
		Texture m_Flat;
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
			m_HeightMap.Load(ASSETPATH("terrain_height.png"));
			m_RGBMap.Load(ASSETPATH("terrain_rgb.png"));
			m_Flat.Load(ASSETPATH("flat.png"));
			LoadModel(&m_airportModel, ASSETPATH("airport.obj"));

			for (auto& mesh : m_airportModel.mergedMesh)
			{
				if (mesh.vertices.size() <= 0)
					continue;
				MeshRenderer mr;
				mr.Set(&mesh);
				m_airportMeshRenderers.push_back(mr);
			}
		}


		virtual void Render() override
		{
			Empty3D::Render();
			m_TerrainShader.Use();
			m_TerrainShader.Set("cam", m_Camera.GetMatrix());
			m_TerrainShader.Set("hightmap", m_HeightMap);
			m_TerrainShader.Set("flatenmap", m_Flat);
			m_TerrainShader.Set("rgbmap", m_RGBMap);
			m_TerrainMeshRenderer.Draw();

			m_AirportShader.Use();
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(0.0f, -200.0f);
			glDepthMask(false);
			Matrix4x4 mat = m_Camera.GetMatrix();
			mat = glm::translate(mat, Vector3{ -170,34,-20 });
			mat = glm::scale(mat, Vector3{0.03,0.03,0.03});

			m_AirportShader.Set("_mvp", mat);
			m_AirportShader.Set("diffuse", Color{ 0.674510, 0.674510, 0.674510,1.0 });
			m_airportMeshRenderers[0].Draw();
			m_AirportShader.Set("diffuse", Color{ 0.800000, 0.439216, 0.203922,1.0 });
			m_airportMeshRenderers[1].Draw();
			m_AirportShader.Set("diffuse", Color{ 1.000000, 1.000000, 1.000000,1.0 });
			m_airportMeshRenderers[2].Draw();

			glDisable(GL_POLYGON_OFFSET_FILL);
			glDepthMask(true);
		}
	};
}


RUN_WINDOW(example::FlightSimulator)