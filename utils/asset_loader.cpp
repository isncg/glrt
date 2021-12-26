#include "log.h"
#include "asset_loader.h"
#include <iostream>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <bsp30/BSPLoader.h>
#pragma comment(lib, "assimp-vc142-mt.lib")
#include <wad/WADFile.h>
#include <exception>
#include "stdhelpers.h"
bool LoadMesh(Mesh* output, aiMesh* input)
{
	output->pMaterialInfo = new MeshMaterialInfo();
	output->pMaterialInfo->materialid = input->mMaterialIndex;
    for (int i = 0; i < input->mNumVertices; i++)
    {
        if (input->mVertices != NULL)
            output->vertices.push_back(Vector3{ input->mVertices[i].x, input->mVertices[i].y, input->mVertices[i].z });

        if (input->mNormals != NULL)
            output->normals.push_back(Vector3{ input->mNormals[i].x, input->mNormals[i].y, input->mNormals[i].z });

        if (input->mTangents != NULL)
            output->tangents.push_back(Vector4{ input->mTangents[i].x, input->mTangents[i].y, input->mTangents[i].z, 1.0f});

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
        for (int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS && j< std::end(uvset) - std::begin(uvset); j++)
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
    for (int f = 0; f < input->mNumFaces; f++)
    {
        aiFace& face = input->mFaces[f];
        for (int i = 0; i < face.mNumIndices - 2; i++)
        {
			output->triangles.push_back(face.mIndices[i]);
			output->triangles.push_back(face.mIndices[i + 1]);
			output->triangles.push_back(face.mIndices[i + 2]);
        }
    }
    return true;
}

void DumpMesh(Mesh* m)
{
    std::cout << m->vertices.size() << std::endl;
    if (m->vertices.size() > 0)
    {
        std::cout << "vertices" << std::endl;
        for (auto& v : m->vertices)
            std::cout << "\t" << v.x << "\t" << v.y << "\t" << v.z << std::endl;
    }
    if (m->normals.size() > 0)
    {
        std::cout << "normals" << std::endl;
        for (auto& v : m->normals)
            std::cout << "\t" << v.x << "\t" << v.y << "\t" << v.z << std::endl;
    }
    if (m->tangents.size() > 0)
    {
        std::cout << "tangents" << std::endl;
        for (auto& v : m->tangents)
            std::cout << "\t" << v.x << "\t" << v.y << "\t" << v.z << "\t" << v.w << std::endl;
    }
    if (m->uv.size() > 0)
    {
        std::cout << "uv" << std::endl;
        for (auto& v : m->uv)
            std::cout << "\t" << v.x << "\t" << v.y << std::endl;
    }
    if (m->uv2.size() > 0)
    {
        std::cout << "uv2" << std::endl;
        for (auto& v : m->uv2)
            std::cout << "\t" << v.x << "\t" << v.y << std::endl;
    }
    if (m->uv3.size() > 0)
    {
        std::cout << "uv3" << std::endl;
        for (auto& v : m->uv3)
            std::cout << "\t" << v.x << "\t" << v.y << std::endl;
    }
    if (m->uv4.size() > 0)
    {
        std::cout << "uv4" << std::endl;
        for (auto& v : m->uv4)
            std::cout << "\t" << v.x << "\t" << v.y << std::endl;
    }
    if (m->uv5.size() > 0)
    {
        std::cout << "uv5" << std::endl;
        for (auto& v : m->uv5)
            std::cout << "\t" << v.x << "\t" << v.y << std::endl;
    }
    if (m->uv6.size() > 0)
    {
        std::cout << "uv6" << std::endl;
        for (auto& v : m->uv6)
            std::cout << "\t" << v.x << "\t" << v.y << std::endl;
    }
    if (m->uv7.size() > 0)
    {
        std::cout << "uv7" << std::endl;
        for (auto& v : m->uv7)
            std::cout << "\t" << v.x << "\t" << v.y << std::endl;
    }
    if (m->uv8.size() > 0)
    {
        std::cout << "uv8" << std::endl;
        for (auto& v : m->uv8)
            std::cout << "\t" << v.x << "\t" << v.y << std::endl;
    }
    if (m->colors.size() > 0)
    {
        std::cout << "colors" << std::endl;
        for (auto& v : m->colors)
            std::cout << "\t" << v.r << "\t" << v.g << "\t" << v.b << "\t" << v.a << std::endl;
    }
    if (m->colors32.size() > 0)
    {
        std::cout << "colors32" << std::endl;
        for (auto& v : m->colors32)
            std::cout << "\t" << v.r << "\t" << v.g << "\t" << v.b << "\t" << v.a << std::endl;
    }
    if (m->triangles.size() > 0)
    {
        std::cout << "triangles" << std::endl;
        for (auto& v : m->triangles)
            std::cout << "\t" <<v<< std::endl;
    }
}


bool LoadModel(Model* output, std::string file)
{
    // Create an instance of the Importer class
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll 
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(file,
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

    for (int i = 0; i < scene->mNumMaterials; i++)
    {
        Mesh mesh;
		mesh.pMaterialInfo = new MeshMaterialInfo{ i, std::string(scene->mMaterials[i]->GetName().C_Str()) };
		output->mergedMesh.push_back(mesh);
		output->matInfos.push_back(mesh.pMaterialInfo);
    }

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		Mesh mesh;
		LoadMesh(&mesh, scene->mMeshes[i]);
		std::cout << "Loaded mesh " << file << "[" << i << "] vertices:" << mesh.vertices.size() << " indices:" << mesh.triangles.size() << std::endl;
		if (mesh.vertices.size() > 0 && mesh.triangles.size() > 0)
		{
		    //DumpMesh(&mesh);
			output->meshCollection.push_back(mesh);
            output->mergedMesh[mesh.pMaterialInfo->materialid].Merge(mesh);
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

//using namespace bsp30;

bsp30::VECTOR3D SwitchHandedness(bsp30::VECTOR3D v) {
    return bsp30::VECTOR3D(v.x, v.z, v.y);
}