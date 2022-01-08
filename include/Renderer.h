#pragma once
#include <framework.h>
#include <string>
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"

struct Sampler
{
    GLuint obj;
    //TBD
};


class Renderer
{
public:
    //std::map<int, Material*> materialPassDict;
    Material* material;
    //void UseMaterial();
    virtual void Draw();
    virtual void Draw(Shader& shader);
};

class MeshRenderer: public Renderer
{
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    bool ValidateMesh(Mesh* pMesh);
    std::vector<unsigned int> indices;
    size_t triangleCount;
public:
    void Set(Mesh* pMesh);
    virtual void Draw() override;
    virtual void Draw(Shader& shader) override;
    static std::vector<MeshRenderer*> CreateRenderers(std::vector<Mesh> meshset);
};

class CanvasRenderer: public Renderer
{
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    std::vector<unsigned int> indices;
    size_t triangleCount;
public:
    void Set(CanvasMesh* pMesh);
    void SetFullScreen();
    virtual void Draw() override;
};
