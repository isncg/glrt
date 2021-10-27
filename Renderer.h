#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

typedef glm::vec3 Vector3;
typedef glm::vec2 Vector2;
typedef glm::vec4 Vector4;
typedef glm::mat4 Matrix4x4;
typedef glm::vec4 Color;
typedef glm::lowp_i8vec4 Color32;

template<typename T>
GLint GetComponentCount(const T& tag);
GLint GetComponentCount(const Color32& tag);

GLenum GetComponentType(const Vector3& tag);
GLenum GetComponentType(const Vector2& tag);
GLenum GetComponentType(const Vector4& tag);
GLenum GetComponentType(const Color32& tag);

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

template<typename T>
inline GLint GetComponentCount(const T& tag)
{
    return T::length();
}
