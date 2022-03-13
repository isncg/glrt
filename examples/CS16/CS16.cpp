#include <Application.h>
#include <Renderer.h>
#include <RenderTarget.h>
#include <Light.h>
#include <imgui/imgui.h>
#include <bsp30/BSPLoader.h>
#include <wad/WADFile.h>

void HLMapLoad(std::string bsp, std::string wad, Model& outputModel, std::map<std::string, Texture>* outputTextures);

class CS16 : public Viewer3D
{
    ASSETDIR("assets/");
    Model						m_LevelModel;
    Shader* m_LevelShader;
    Texture						m_LevelPlaceHolderTexture;
    std::vector<MeshRenderer*>	m_LevelSubmeshRenderers;	//VAOs for each submesh of bsp map model
    std::vector<Material>		m_LevelSubmeshMaterials;	//material for each submesh of bsp map model
    RenderPass					m_LevelPass;
    RenderPass                  m_PostEffectPass;
    Shader* m_PostEffectShader;
    CanvasRenderer				m_PostEffectRenderer;
    DirectionalLight			m_ShadowMappingLight;
    Shader* m_psm;

    Vector3 lightPos{ -500,2000,1000 };
    Vector3 lightDir{ 1,-4,-2 };
    float lightRange = 1024;
    virtual void Start() override
    {
        Viewer3D::Start();
        std::map<std::string, Texture> textureDict;

        HLMapLoad(ASSETPATH("de_dust2.bsp"), ASSETPATH("cs_dust.wad"), m_LevelModel, &textureDict);

        m_LevelPlaceHolderTexture.Load(ASSETPATH("256.bmp"));

        m_ShadowMappingLight.InitLightMap(8192, 8192);
        m_ShadowMappingLight.SetLight(lightPos, lightDir, lightRange);
        m_psm = ShaderLib::Instance().Load(ASSETPATH("glsl/shadowmapping"));
        m_LevelShader = ShaderLib::Instance().Load(ASSETPATH("glsl/bsplevel"));


        m_LevelPass.Init(4096, 2048, 1, true);
        auto& mtl = MaterialLib::Instance();
        for (auto& kv : textureDict)
        {
            mtl.Add(m_LevelShader, kv.first, [&](Material& mat) {
                mat.Set("tex", kv.second);
                });
        }

        mtl.Add(m_LevelShader, "default", [&](Material& mat) {
            mat.Set("tex", m_LevelPlaceHolderTexture);
            });

        m_LevelSubmeshRenderers = MeshRenderer::CreateRenderers(m_LevelModel.meshCollection);

        m_LevelModel.Clear();

        GlobalMaterial::Instance().Set("g_shadow", m_ShadowMappingLight.m_ShadowMappingPass.depthBuffer);
        GlobalMaterial::Instance().Set("g_lightcolor", ColorRGB{ 1.0,1.0,1.0 });
        GlobalMaterial::Instance().Set("g_shadowbias", 0.0000005f);
        GlobalMaterial::Instance().Set("g_ambient", 0.4f);

        m_CamController.speed = 100;
    }

    virtual void RenderScene() override
    {
        Viewer3D::RenderScene();
        Matrix4x4 maptrans(1);
        m_psm->Set("world", maptrans);
        m_LevelShader->Set("world", maptrans);

        m_ShadowMappingLight.m_ShadowMappingPass.Bind();
        for (auto& r : m_LevelSubmeshRenderers)
            r->Draw(*m_psm);
        RenderPass::BindDefault(this);
        for (auto& r : m_LevelSubmeshRenderers)
            r->Draw();

        m_ShadowMappingLight.SetLight(lightPos + m_CamController.position, lightDir, lightRange);
        GlobalMaterial::Instance().Set("g_light", m_ShadowMappingLight.matrix);
    }

    virtual void OnImGUI() override
    {
        Viewer3D::OnImGUI();
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


            ImGui::Begin("Shadow mapping");
            ImGui::Image((ImTextureID)this->m_ShadowMappingLight.m_ShadowMappingPass.depthBuffer.id, ImVec2{ 512,512 });
            ImGui::End();

            MaterialLibInspector::Default().OnInspector();
            GlobalMaterial::Instance().OnInspector();
        }
    }
}_;


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
    for (int faceId = model->iFirstFace; faceId < (model->iFirstFace + model->nFaces); faceId++)
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
            pMesh = (outputModel.meshCollection.end() - 1)._Ptr;
            pMesh->pMaterialInfo = new MeshMaterialInfo{ (int)outputModel.matInfos.size(), tex->szName };
            outputModel.matInfos.push_back(pMesh->pMaterialInfo);
            outputModel.meshDict[tex->szName] = (int)outputModel.meshCollection.size() - 1;
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
                pMesh->triangles.push_back((unsigned int)pMesh->vertices.size());
                pMesh->vertices.push_back(p_);
                pMesh->normals.push_back(normal);
                pMesh->uv.push_back({ glm::dot(uvscale * p_ , texVS) + texInfo.fSShift, glm::dot(uvscale * p_ , texVT) + texInfo.fTShift });

                pMesh->triangles.push_back((unsigned int)pMesh->vertices.size());
                pMesh->vertices.push_back(p0);
                pMesh->normals.push_back(normal);
                pMesh->uv.push_back({ glm::dot(uvscale * p0 , texVS) + texInfo.fSShift, glm::dot(uvscale * p0 , texVT) + texInfo.fTShift });

                pMesh->triangles.push_back((unsigned int)pMesh->vertices.size());
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
            texture.target = GL_TEXTURE_2D;
            texture.name = name;
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
                texture.name = name;
                texture.target = GL_TEXTURE_2D;
                (*outputTextures)[name] = texture;
                wadTexture.Release();
            }
        }
    }
}
