#pragma once
#include "framework.h"
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
    VE_NORMALS  = 2,
    VE_TANGENTS = 4,
    VE_UV       = 8,
    VE_UV2      = 16,
    VE_UV3      = 32,
    VE_UV4      = 64,
    VE_UV5      = 128,
    VE_UV6      = 256,
    VE_UV7      = 512,
    VE_UV8      = 1024,
    VE_COLORS   = 2048,
    VE_COLOR32S = 4096
};

struct Mesh
{
	int vertexBufferCount;
	int blendShapeCount;
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
    int vertexAttributeCount;
    std::vector<unsigned int> triangles;

    int GetBufferSize();
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
    std::vector<std::string> matNames;
    std::map<std::string, int> meshDict;
    ModelTreeNode root;
};

struct Sampler
{
    GLuint obj;
    //TBD
};




class Renderer
{
public:
    std::map<int, Material*> materialPassDict;
    Material* material;
    void UseMaterial(int passID);
    virtual void Draw(int passID = -1) = 0;
};

class MeshRenderer: public Renderer
{
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    bool ValidateMesh(Mesh* pMesh);
    std::vector<unsigned int> indices;
    int triangleCount;
public:
    void Set(Mesh* pMesh);
    void Draw(int passID = -1);
};

template<typename T>
inline GLint GetComponentCount(const T& tag)
{
    return T::length();
}

typedef struct tagCanvasRect
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
    void setWidth(float width, float pivot);
    void setHeight(float width, float pivot);
    void setSize(Vector2 size, Vector2 pivot);
}CanvasRect;

typedef struct tagCanvasMesh
{
    std::vector<Vector2> vertices;
    std::vector<Vector2> uv;
    std::vector<Vector2> uv2;

    void Set(CanvasRect& rect, float halfWidth, float halfHeight);
    int GetBufferSize();
    void MergeBatch(tagCanvasMesh& mesh);
}CanvasMesh;

class CanvasRenderer: public Renderer
{
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    std::vector<unsigned int> indices;
    int triangleCount;
public:
    void Set(CanvasMesh* pMesh);
    void SetFullScreen();
    void Draw(int passID = -1);
};
