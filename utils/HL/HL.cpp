#include "HL.h"
#include "../../include/Application.h"

bool LoadBSPMap(Model& outputModel, std::map<std::string, Texture>* outputTextures, const char* fnbsp)
{
    bsp30::BSPLoader loader(fnbsp);
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

    //Mesh mesh;

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
        bsp30::BSPMIPTEX* tex = m_bspLoader->m_Textures[texInfo.iMiptex];

        Vector3 texVS{ texInfo.vS.x, texInfo.vS.z, -texInfo.vS.y };
        Vector3 texVT{ texInfo.vT.x, texInfo.vT.z, -texInfo.vT.y };
        //	skyboxes are not to be added to our visible mesh
        if (!strcmp(tex->szName, "sky")) {
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

        Mesh* pMesh = nullptr;
        auto meshit = outputModel.meshDict.find(tex->szName);
        if (meshit == outputModel.meshDict.end())
        {
            Mesh mr;
            outputModel.meshCollection.push_back(mr);
            outputModel.matNames.push_back(tex->szName);
            pMesh = (outputModel.meshCollection.end() - 1)._Ptr;
            outputModel.meshDict[tex->szName] = outputModel.meshCollection.size() - 1;
        }
        else
        {
            pMesh = &outputModel.meshCollection[meshit->second];
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
            else if (surfedgeId < (face->iFirstEdge + face->nEdges - 1))
            {
                Vector3 p_{ v_.x, v_.z, -v_.y };
                Vector3 p0{ v0.x, v0.z, -v0.y };
                Vector3 p1{ v1.x, v1.z, -v1.y };

                Vector3 normal = glm::normalize(glm::cross(p0 - p_, p0 - p1));
                float uvscale = 1.0;
                pMesh->triangles.push_back(pMesh->vertices.size());
                pMesh->vertices.push_back(p_);
                pMesh->normals.push_back(normal);
                pMesh->uv.push_back({ glm::dot(uvscale * p_ , texVS) + texInfo.fSShift, glm::dot(uvscale * p_ , texVT) + texInfo.fTShift });

                pMesh->triangles.push_back(pMesh->vertices.size());
                pMesh->vertices.push_back(p0);
                pMesh->normals.push_back(normal);
                pMesh->uv.push_back({ glm::dot(uvscale * p0 , texVS) + texInfo.fSShift, glm::dot(uvscale * p0 , texVT) + texInfo.fTShift });

                pMesh->triangles.push_back(pMesh->vertices.size());
                pMesh->vertices.push_back(p1);
                pMesh->normals.push_back(normal);
                pMesh->uv.push_back({ glm::dot(uvscale * p1 , texVS) + texInfo.fSShift, glm::dot(uvscale * p1 , texVT) + texInfo.fTShift });
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

    if (nullptr != outputTextures)
    {
        for (auto& item : loader.m_internalTextures)
        {
            WadTexture_C wadTexture;
            wadTexture.Init((uint8_t*)item);
            char* name = wadTexture.meta->name;
            if (outputTextures->find(name) != outputTextures->end())
            {
                wadTexture.Release();
                continue;
            }
            Texture texture;
            glGenTextures(1, &texture.id);
            glBindTexture(GL_TEXTURE_2D, texture.id);
            for (int level = 0; level < 4; level++)
            {
                auto mip = wadTexture.mipmaps[level];
                glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, wadTexture.meta->width >> level, wadTexture.meta->height >> level, 0, GL_RGBA, GL_UNSIGNED_BYTE, mip);
            }
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            texture.handle = glGetTextureHandleARB(texture.id);
            (*outputTextures)[name] = texture;
            wadTexture.Release();
        }
    }
    return true;
}


void HLMapLoad(std::string fnbsp, std::string fnwad, Model& outputModel, std::map<std::string, Texture>* outputTextures)
{
    //BSP
    LoadBSPMap(outputModel, outputTextures, fnbsp.c_str());


	// WAD
    if (nullptr != outputTextures)
    {
        WadFile wad;
        wad.Load(fnwad);
        for (auto& item : wad.items)
        {
            if (item.meta.type == 'C')
            {
                char* name = item.meta.name;
                if (outputTextures->find(name) != outputTextures->end())
                    continue;
                Texture texture;
                glGenTextures(1, &texture.id);
                glBindTexture(GL_TEXTURE_2D, texture.id);
                auto wadTexture = item.textureC;
                wadTexture.Init(item.buffer);
                for (int level = 0; level < 4; level++)
                {
                    auto mip = wadTexture.mipmaps[level];
                    glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, wadTexture.meta->width >> level, wadTexture.meta->height >> level, 0, GL_RGBA, GL_UNSIGNED_BYTE, mip);
                }
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                texture.handle = glGetTextureHandleARB(texture.id);
                (*outputTextures)[name] = texture;
                wadTexture.Release();
            }
        }
    }
}
