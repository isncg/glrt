#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../empty3d/empty3d.h"

namespace example
{
	class Terrain: public Empty3D
	{
		ASSETDIR("examples/terrain/assets/");
		Mesh m_TerrainMesh;
		MeshRenderer m_TerrainMeshRenderer;
		Shader m_TerrainShader;
		Texture m_HeightMap;
		virtual void OnCreate() override
		{
			Empty3D::OnCreate();
			int size = 1000;
			for (int x = 0; x <= size; x++)
			{
				for (int z = 0; z <= size; z++)
				{
					m_TerrainMesh.vertices.push_back(Vector3{ (float)x / (float)size, 0, (float)z / (float)size });
					m_TerrainMesh.normals.push_back(Vector3{ 0,0,0 });
				}
			}
			for (int x = 0; x < size; x++)
			{
				for (int z = 0; z < size; z++)
				{
					int i0 = (x + 0) * (size + 1) + (z + 0);
					int i1 = (x + 0) * (size + 1) + (z + 1);
					int i2 = (x + 1) * (size + 1) + (z + 1);
					int i3 = (x + 1) * (size + 1) + (z + 0);
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
			m_HeightMap.Load(ASSETPATH("terrain_height.png"));
		}

		virtual void Render() override
		{
			Empty3D::Render();
			m_TerrainShader.Use();
			m_TerrainShader.Set("cam", m_Camera.GetMatrix());
			m_TerrainShader.Set("hightmap", m_HeightMap);
			m_TerrainMeshRenderer.Draw();
		}
	};
}

//RUN_WINDOW(example::Terrain)