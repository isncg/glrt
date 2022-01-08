#pragma once
#include <string>
#include "Texture.h"
#include "Material.h"

enum IndexFormat
{
    UInt16 = 0,
    UInt32 = 1
};

enum VertexElement
{
    VE_VERTICES = 1,
    VE_NORMALS = 2,
    VE_TANGENTS = 4,
    VE_UV = 8,
    VE_UV2 = 16,
    VE_UV3 = 32,
    VE_UV4 = 64,
    VE_UV5 = 128,
    VE_UV6 = 256,
    VE_UV7 = 512,
    VE_UV8 = 1024,
    VE_COLORS = 2048,
    VE_COLOR32S = 4096
};

struct MeshMaterialInfo
{
    int materialid = 0;
    std::string name;
};


struct Mesh
{
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector4> tangents;
    std::vector<Vector2> uv;
    std::vector<Vector2> uv2;
    std::vector<Vector2> uv3;
    std::vector<Vector2> uv4;
    std::vector<Vector2> uv5;
    std::vector<Vector2> uv6;
    std::vector<Vector2> uv7;
    std::vector<Vector2> uv8;
    std::vector<Color> colors;
    std::vector<Color32> colors32;
    //int vertexAttributeCount;
    std::vector<unsigned int> triangles;
    MeshMaterialInfo* pMaterialInfo = nullptr;
    size_t GetBufferSize();
    void Clear();
    void Merge(Mesh& other);
};

struct ModelTreeNode
{
    Matrix4x4 transform;
    std::vector<int> meshIndices;
    std::vector<ModelTreeNode*> children;
};
struct Model
{
    std::vector<Mesh> meshCollection;
    std::vector<Mesh> mergedMesh;
    std::vector<MeshMaterialInfo*> matInfos;
    std::map<std::string, int> meshDict;
    ModelTreeNode root;
    void Clear();
    bool Load(std::string filename);
};

struct CanvasRect
{
    Vector2 pos;
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float width();
    float height();
    float pivotX();
    float pivotY();
    Vector2 pivot();
    CanvasRect();
    CanvasRect(Vector2 pos, Vector2 size, Vector2 pivot);
    void SetRect(Vector2 pos, Vector2 size, Vector2 pivot);
};


struct CanvasMesh
{
    std::vector<Vector2> vertices;
    std::vector<Vector2> uv;
    std::vector<Vector2> uv2;

    void Set(CanvasRect&& rect, Vector2 halfsize);
    size_t GetBufferSize();
    void MergeBatch(CanvasMesh& mesh);
    void MergeBatch(CanvasRect&& rect, Vector2 halfsize);
};