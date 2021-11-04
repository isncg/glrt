#include "log.h"
#include "asset_loader.h"
#include <iostream>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <bsp30/BSPLoader.h>
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
		std::cout << "Loaded mesh " << file << "[" << i << "] vertices:" << mesh.vertices.size() << " indices:" << mesh.triangles.size() << std::endl;
		if (mesh.vertices.size() > 0 && mesh.triangles.size() > 0)
		{
		    //DumpMesh(&mesh);
			output->meshCollection.push_back(mesh);
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

bool LoadBSPMap(Model* output, const char* file)
{
    bsp30::BSPLoader loader(file);
    loader.ReadVertices();
    loader.ReadPlanes();
    loader.ReadEdges();
    loader.ReadSurfEdges();
    loader.ReadTexInfo();
    loader.ReadTextures();
    loader.ReadFaces();
    loader.ReadModels();
    loader.ReadEntities();

   
    auto model = loader.m_Models;
    unsigned nPolygons = model->nFaces;
    auto m_bspLoader = &loader;
    
    Mesh mesh;
    
    // Go through all the faces of BSPMODEL
    for (unsigned faceId = model->iFirstFace; faceId < (model->iFirstFace + model->nFaces); faceId++) 
    {
        // A face is a n-sided-polygon defined by a series of "SurfEdges"
        // It's basically a closed loop of edges like this:
        // f : (e0, e1, e2, ..., eN-1) for N edges
        // f : (v0 -> v1, v1 -> v2, v2 -> v3, ... , vN-1 -> v0) for N vertices
        // so number of vertices aka Control Points is same as number of surfedges 
        bsp30::BSPFACE* face = &(m_bspLoader->m_Faces[faceId]);

        // Get face's material
        bsp30::BSPTEXTUREINFO texInfo = m_bspLoader->m_TextureInfos[face->iTextureInfo];
        bsp30::BSPMIPTEX tex = m_bspLoader->m_Textures[texInfo.iMiptex];

        Vector3 texVS{ texInfo.vS.x, texInfo.vS.z, -texInfo.vS.y };
        Vector3 texVT{ texInfo.vT.x, texInfo.vT.z, -texInfo.vT.y };
        //	skyboxes are not to be added to our visible mesh
        if (!strcmp(tex.szName, "sky")) {
            // We're a polygon less now
            nPolygons--;
            continue;
        }


        // Store each face's normal
        bsp30::BSPPLANE* plane = &(m_bspLoader->m_Planes[face->iPlane]);
        bsp30::VECTOR3D normal = plane->vNormal;
        // Reverse normal if specified
        if (face->nPlaneSide) {
            normal.x *= -1.0f;
            normal.y *= -1.0f;
            normal.z *= -1.0f;
        }
      

        // For each surfedge
        bsp30::VECTOR3D v_;
        for (unsigned surfedgeId = face->iFirstEdge; surfedgeId < (face->iFirstEdge + face->nEdges); surfedgeId++) {

        // Get corresponding edgeId
            int edgeId = m_bspLoader->m_SurfEdges[surfedgeId];
            unsigned edgeId_abs = abs(edgeId);

        // Get vertex IDs of edge
            unsigned v0Id = m_bspLoader->m_Edges[edgeId_abs].iVertex[0];
            unsigned v1Id = m_bspLoader->m_Edges[edgeId_abs].iVertex[1];
        // Swap v0Id and v1Id if we have a negative edgeId
            if (edgeId < 0)
                bsp30::swap(v0Id, v1Id);
            // Get vertices
            bsp30::VECTOR3D v0 = m_bspLoader->m_Vertices[v0Id];
            bsp30::VECTOR3D v1 = m_bspLoader->m_Vertices[v1Id];
            if (surfedgeId == face->iFirstEdge)
            {
                v_ = v0;
            }
			else if(surfedgeId < (face->iFirstEdge + face->nEdges -1))
            {
                Vector3 p_{ v_.x, v_.z, -v_.y };
                Vector3 p0{ v0.x, v0.z, -v0.y };
                Vector3 p1{ v1.x, v1.z, -v1.y };

                Vector3 normal = glm::normalize(glm::cross(p0 - p_, p0 - p1));
                float uvscale = 0.005f;
                mesh.triangles.push_back(mesh.vertices.size());
                mesh.vertices.push_back(p_);
                mesh.normals.push_back(normal);
				mesh.uv.push_back({ glm::dot(uvscale *p_ , texVS) + texInfo.fSShift, glm::dot(uvscale * p_ , texVT) + texInfo.fTShift });

                mesh.triangles.push_back(mesh.vertices.size());
                mesh.vertices.push_back(p0);
                mesh.normals.push_back(normal);
                mesh.uv.push_back({ glm::dot(uvscale * p0 , texVS) + texInfo.fSShift, glm::dot(uvscale * p0 , texVT) + texInfo.fTShift });
                
                mesh.triangles.push_back(mesh.vertices.size());
                mesh.vertices.push_back(p1);
                mesh.normals.push_back(normal);
                mesh.uv.push_back({ glm::dot(uvscale * p1 , texVS) + texInfo.fSShift, glm::dot(uvscale * p1 , texVT) + texInfo.fTShift });
            }

        // Every edge naturally defines a tangent as well
            bsp30::VECTOR3D tangent;
            tangent.x = v0.x - v1.x;
            tangent.y = v0.y - v1.y;
            tangent.z = v0.z - v1.z;

            float tangent_length = sqrt(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);
            tangent.x = tangent.x / tangent_length;
            tangent.y = tangent.y / tangent_length;
            tangent.z = tangent.z / tangent_length;

        // Compute texture coordinates for this vertex
        // http://www.flipcode.com/archives/Quake_2_BSP_File_Format.shtml
            float u = (texInfo.vS.x * v0.x + texInfo.vS.y * v0.y + texInfo.vS.z * v0.z) + texInfo.fSShift;
            float v = (texInfo.vT.x * v0.x + texInfo.vT.y * v0.y + texInfo.vT.z * v0.z) + texInfo.fTShift;
        }
    }    
   
    output->meshCollection.push_back(mesh);
    return true;
}


#include <freeimage/FreeImage.h>
#pragma comment(lib, "FreeImage.lib")
template <class T> void INPLACESWAP(T& a, T& b) {
    a ^= b; b ^= a; a ^= b;
}

BOOL SwapRedBlue32(FIBITMAP* dib) {
    if (FreeImage_GetImageType(dib) != FIT_BITMAP) {
        return FALSE;
    }

    const unsigned bytesperpixel = FreeImage_GetBPP(dib) / 8;
    if (bytesperpixel > 4 || bytesperpixel < 3) {
        return FALSE;
    }

    const unsigned height = FreeImage_GetHeight(dib);
    const unsigned pitch = FreeImage_GetPitch(dib);
    const unsigned lineSize = FreeImage_GetLine(dib);

    BYTE* line = FreeImage_GetBits(dib);
    for (unsigned y = 0; y < height; ++y, line += pitch) {
        for (BYTE* pixel = line; pixel < line + lineSize; pixel += bytesperpixel) {
            INPLACESWAP(pixel[0], pixel[2]);
        }
    }

    return TRUE;
}

bool LoadTexture(Texture* output, const char* filename)
{
    //image format
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    //pointer to the image, once loaded
    FIBITMAP* dib(0);
    //pointer to the image data
    BYTE* bits(0);
    //image width and height
    unsigned int width(0), height(0);
    //OpenGL's image ID to map to
    GLuint gl_texID;

    //check the file signature and deduce its format
    fif = FreeImage_GetFileType(filename, 0);
    //if still unknown, try to guess the file format from the file extension
    if (fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(filename);
    //if still unkown, return failure
    if (fif == FIF_UNKNOWN)
        return false;

    //check that the plugin has reading capabilities and load the file
    if (FreeImage_FIFSupportsReading(fif))
        dib = FreeImage_Load(fif, filename);
    //if the image failed to load, return failure
    if (!dib)
        return false;
    SwapRedBlue32(dib);
    //retrieve the image data
    bits = FreeImage_GetBits(dib);
    //get the image width and height
    width = FreeImage_GetWidth(dib);
    height = FreeImage_GetHeight(dib);
    //if this somehow one of these failed (they shouldn't), return failure
    if ((bits == 0) || (width == 0) || (height == 0))
        return false;

    //if this texture ID is in use, unload the current texture
   /* if (m_texID.find(texID) != m_texID.end())
        glDeleteTextures(1, &(m_texID[texID]));*/

    //generate an OpenGL texture ID for this texture
    glGenTextures(1, &gl_texID);
    //store the texture ID mapping
    //m_texID[texID] = gl_texID;
    //bind to the new texture ID
    glBindTexture(GL_TEXTURE_2D, gl_texID);
    //store the texture data for OpenGL use
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
        0, GL_RGB, GL_UNSIGNED_BYTE, bits);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Free FreeImage's copy of the data
    FreeImage_Unload(dib);
    output->id = gl_texID;
    return true;
}


