#pragma once
#include <vector>
#include <glad/glad.h>
typedef struct stVector3
{
    float x;
    float y;
    float z;
}Vector3;

typedef struct stVector4
{
    float x;
    float y;
    float z;
    float w;
}Vector4;

typedef struct stVector2
{
    float x;
    float y;
}Vector2;

typedef struct stColor
{
    float r;
    float g;
    float b;
    float a;
}Color;

typedef struct stColor32
{
    int8_t r;
    int8_t g;
    int8_t b;
    int8_t a;
}Color32;

GLint GetComponentCount(Vector3 tag);
GLint GetComponentCount(Vector2 tag);
GLint GetComponentCount(Vector4 tag);
GLint GetComponentCount(Color   tag);
GLint GetComponentCount(Color32 tag);

GLenum GetComponentType(Vector3 tag);
GLenum GetComponentType(Vector2 tag);
GLenum GetComponentType(Vector4 tag);
GLenum GetComponentType(Color   tag);
GLenum GetComponentType(Color32 tag);

typedef union stMatrix4x4
{
    float m[16];
    struct
    {
        float m00;
        float m33;
        float m23;
        float m13;
        float m03;
        float m32;
        float m22;
        float m02;
        float m12;
        float m21;
        float m11;
        float m01;
        float m30;
        float m20;
        float m10;
        float m31;
    };
}Matrix4x4;

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
    ModelTreeNode root;
};


class Shader
{
    GLuint program;
public:
    void Load(const char* vert, const char* frag);
    void Use();
};


class Renderer
{
};


class MeshRenderer
{
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    bool ValidateMesh(Mesh* pMesh);
    std::vector<unsigned int> indices;
    int triangleCount;
public:
    void Set(Mesh* pMesh);
    void Draw();
};