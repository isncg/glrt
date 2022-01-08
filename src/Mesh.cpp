#include "../include/Mesh.h"
#include "../utils/utils.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#pragma comment(lib, "assimp-vc142-mt.lib")


size_t Mesh::GetBufferSize()
{
	return
		vectorsizeof(vertices) +
		vectorsizeof(normals) +
		vectorsizeof(tangents) +
		vectorsizeof(uv) +
		vectorsizeof(uv2) +
		vectorsizeof(uv3) +
		vectorsizeof(uv4) +
		vectorsizeof(uv5) +
		vectorsizeof(uv6) +
		vectorsizeof(uv7) +
		vectorsizeof(uv8) +
		vectorsizeof(colors) +
		vectorsizeof(colors32);
}

void Mesh::Clear()
{
	vertices.clear();
	normals.clear();
	tangents.clear();
	uv.clear();
	uv2.clear();
	uv3.clear();
	uv4.clear();
	uv5.clear();
	uv6.clear();
	uv7.clear();
	uv8.clear();
	colors.clear();
	colors32.clear();
	triangles.clear();
}

#define MERGE_COMPONENT(c) this->c.insert(this->c.end(), other.c.begin(), other.c.end())

void Mesh::Merge(Mesh& other)
{
	auto indicesoffset = this->vertices.size();
	MERGE_COMPONENT(vertices);
	MERGE_COMPONENT(normals);
	MERGE_COMPONENT(tangents);
	MERGE_COMPONENT(uv);
	MERGE_COMPONENT(uv2);
	MERGE_COMPONENT(uv3);
	MERGE_COMPONENT(uv4);
	MERGE_COMPONENT(uv5);
	MERGE_COMPONENT(uv6);
	MERGE_COMPONENT(uv7);
	MERGE_COMPONENT(uv8);
	MERGE_COMPONENT(colors);
	MERGE_COMPONENT(colors32);

	for (auto i : other.triangles)
	{
		this->triangles.push_back((unsigned int)(indicesoffset + i));
	}
}

void CanvasMesh::Set(CanvasRect&& rect, Vector2 halfsize)
{
	vertices.clear();
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMin) / halfsize.x, (rect.pos.y + rect.yMin) / halfsize.y });
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMax) / halfsize.x, (rect.pos.y + rect.yMin) / halfsize.y });
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMax) / halfsize.x, (rect.pos.y + rect.yMax) / halfsize.y });
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMin) / halfsize.x, (rect.pos.y + rect.yMax) / halfsize.y });

	uv.clear();
	uv.push_back(Vector2{ 0,0 });
	uv.push_back(Vector2{ 1,0 });
	uv.push_back(Vector2{ 1,1 });
	uv.push_back(Vector2{ 0,1 });

	uv2.clear();
	uv2.push_back(Vector2{ 0,0 });
	uv2.push_back(Vector2{ 1,0 });
	uv2.push_back(Vector2{ 1,1 });
	uv2.push_back(Vector2{ 0,1 });
}

size_t CanvasMesh::GetBufferSize()
{
	return
		vectorsizeof(vertices) +
		vectorsizeof(uv) +
		vectorsizeof(uv2);
}


bool LoadMesh(Mesh* output, aiMesh* input)
{
	output->pMaterialInfo = new MeshMaterialInfo();
	output->pMaterialInfo->materialid = input->mMaterialIndex;
	for (int i = 0; i < (int)input->mNumVertices; i++)
	{
		if (input->mVertices != NULL)
			output->vertices.push_back(Vector3{ input->mVertices[i].x, input->mVertices[i].y, input->mVertices[i].z });

		if (input->mNormals != NULL)
			output->normals.push_back(Vector3{ input->mNormals[i].x, input->mNormals[i].y, input->mNormals[i].z });

		if (input->mTangents != NULL)
			output->tangents.push_back(Vector4{ input->mTangents[i].x, input->mTangents[i].y, input->mTangents[i].z, 1.0f });

		std::vector<Vector2>* uvset[8]{
			&output->uv,
			&output->uv2,
			&output->uv3,
			&output->uv4,
			&output->uv5,
			&output->uv6,
			&output->uv7,
			&output->uv8,
		};
		for (int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS && j < std::end(uvset) - std::begin(uvset); j++)
		{
			if (input->mTextureCoords[j] != NULL)
			{
				auto& uv = input->mTextureCoords[j][i];
				uvset[j]->push_back(Vector2{ uv.x, uv.y });
			}
		}

		std::vector<std::vector<Color>*> colorset
		{
			&output->colors
		};
		for (int j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS && j < colorset.size(); j++)
		{
			if (input->mColors[j] != NULL)
			{
				auto& color = input->mColors[j][i];
				colorset[j]->push_back(Color{ color.r, color.g, color.b, color.a });
			}
		}
	}
	for (unsigned int f = 0; f < input->mNumFaces; f++)
	{
		aiFace& face = input->mFaces[f];
		for (unsigned int i = 0; i < face.mNumIndices - 2; i++)
		{
			output->triangles.push_back(face.mIndices[i]);
			output->triangles.push_back(face.mIndices[i + 1]);
			output->triangles.push_back(face.mIndices[i + 2]);
		}
	}
	return true;
}


bool Model::Load(std::string filename)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!scene)
	{
		log(importer.GetErrorString(), LogLevel::Error);
		return false;
	}
	// Now we can access the file's contents. 

	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		Mesh mesh;
		mesh.pMaterialInfo = new MeshMaterialInfo{ (int)i, std::string(scene->mMaterials[i]->GetName().C_Str()) };
		mergedMesh.push_back(mesh);
		matInfos.push_back(mesh.pMaterialInfo);
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		Mesh mesh;
		LoadMesh(&mesh, scene->mMeshes[i]);
		std::cout << "Loaded mesh " << filename << "[" << i << "] vertices:" << mesh.vertices.size() << " indices:" << mesh.triangles.size() << std::endl;
		if (mesh.vertices.size() > 0 && mesh.triangles.size() > 0)
		{
			//DumpMesh(&mesh);
			meshCollection.push_back(mesh);
			mergedMesh[mesh.pMaterialInfo->materialid].Merge(mesh);
		}
		else
		{
			std::cout << "Mesh invalid" << std::endl;
		}
	}

	//DoTheSceneProcessing(scene);
	// We're done. Everything will be cleaned up by the importer destructor
	return true;
}

void CanvasRect::SetRect(Vector2 pos, Vector2 size, Vector2 pivot)
{
	this->pos = pos;
	xMin = -pivot.x * size.x;
	xMax = (1 - pivot.x) * size.x;
	yMin = -pivot.y * size.y;
	yMax = (1 - pivot.y) * size.y;
}

CanvasRect::CanvasRect()
{
}

CanvasRect::CanvasRect(Vector2 pos, Vector2 size, Vector2 pivot)
{
	SetRect(pos, size, pivot);
}

void CanvasMesh::MergeBatch(CanvasMesh& mesh)
{
	vertices.insert(vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
	uv.insert(uv.end(), mesh.uv.begin(), mesh.uv.end());
	uv2.insert(uv2.end(), mesh.uv2.begin(), mesh.uv2.end());
}

void CanvasMesh::MergeBatch(CanvasRect&& rect, Vector2 halfsize)
{
	CanvasMesh mesh;
	mesh.Set(std::move(rect), halfsize);
	MergeBatch(mesh);
}