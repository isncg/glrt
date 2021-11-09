#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
typedef glm::vec3 Vector3;
typedef glm::vec2 Vector2;
typedef glm::vec4 Vector4;
typedef glm::mat4 Matrix4x4;
typedef glm::vec4 Color;
typedef glm::lowp_i8vec4 Color32;
typedef glm::ivec2 Vector2i;

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

struct Sampler
{
    GLuint obj;
    //TBD
};

struct Texture
{
    GLuint id;
    GLuint64 handle;
};

class Shader
{
	GLuint program;
public:
	void Load(const char* vert, const char* frag);
	void Use();
	void Set(const char* name, Matrix4x4& value);
    void Set(const char* name, Matrix4x4&& value);
    void Set(const char* name, Vector2& value);
    void Set(const char* name, Vector2&& value);
    void Set(const char* name, Vector3& value);
    void Set(const char* name, Vector3&& value);
    void Set(const char* name, float value);
    void Set(const char* name, Texture* texture);
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

class CanvasRenderer
{
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    std::vector<unsigned int> indices;
    int triangleCount;
public:
    void Set(CanvasMesh* pMesh);
    void SetFullScreen();
    void Draw();
};
