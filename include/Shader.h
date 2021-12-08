#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "ResourceMonitor.h"
struct Texture;
class Shader: public IResourceUpdateNotify
{
	friend class Material;
	Material* lastMaterial = NULL;
	GLuint program;
	std::string vert;
	std::string frag;
	
public:
	~Shader();
	//void Load(std::string vert, std::string frag);
	void Load(std::string&& vert, std::string&& frag);
	void Use();
	void Set(const char* name, Matrix4x4& value);
	void Set(const char* name, Matrix4x4&& value);
	void Set(const char* name, Vector2& value);
	void Set(const char* name, Vector2&& value);
	void Set(const char* name, Vector3& value);
	void Set(const char* name, Vector3&& value);
	void Set(const char* name, float value);
	void Set(const char* name, Texture& texture);
	virtual void OnResourceUpdated() override;
};