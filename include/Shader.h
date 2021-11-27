#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
struct Texture;
class Shader
{
	friend class Material;
	Material* lastMaterial = NULL;
	GLuint program;
public:
	void Load(std::string vert, std::string frag);
	void Use();
	void Set(const char* name, Matrix4x4& value);
	void Set(const char* name, Matrix4x4&& value);
	void Set(const char* name, Vector2& value);
	void Set(const char* name, Vector2&& value);
	void Set(const char* name, Vector3& value);
	void Set(const char* name, Vector3&& value);
	void Set(const char* name, float value);
	void Set(const char* name, Texture& texture);
};