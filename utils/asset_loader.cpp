#include "log.h"
#include "asset_loader.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#pragma comment(lib, "assimp-vc142-mt.lib")

bool LoadMesh(Mesh* output, aiMesh* input)
{
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

        std::vector<Color>* colorset[1]{
            &output->colors
        };
        for (int j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS && j < std::end(colorset) - std::begin(colorset); j++)
        {
            if (input->mColors[j] != NULL)
            {
                auto& color = input->mColors[j][i];
                colorset[j]->push_back(Color{ color.r, color.g, color.b, color.a });
            }
        }
    }
    return true;
}

bool LoadModel(Model* output, const char* file)
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

    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        Mesh mesh;
        LoadMesh(&mesh, scene->mMeshes[i]);
        output->meshCollection.push_back(mesh);
    }

    //DoTheSceneProcessing(scene);
    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}


