#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "ResourceMonitor.h"
#include "Script.h"
class Texture;
class Shader: public IResourceUpdateNotify, public IScriptable
{
	friend class Material;
	friend class GlobalMaterial;
	Material* lastMaterial = NULL;
	GLuint program;
	std::string vert;
	std::string frag;
	std::string geom;
public:
	static Shader* Current();
	~Shader();
	//void Load(std::string vert, std::string frag);
	void Load(std::string&& vert, std::string&& frag);
	bool Load(std::string&& vert, std::string&& frag, std::string&& geom);
	void Use();
	void Set(const char* name, Matrix4x4& value);
	void Set(const char* name, Matrix4x4&& value);
	void Set(const char* name, Vector2& value);
	void Set(const char* name, Vector2&& value);
	void Set(const char* name, Vector3& value);
	void Set(const char* name, Vector3&& value);
	void Set(const char* name, Vector4& value);
	void Set(const char* name, Vector4&& value);
	void Set(const char* name, float value);
	void Set(const char* name, Texture& texture);
	virtual void OnResourceUpdated() override;
};

enum ShaderFileType
{
	None = 0,
	Vert = 1,
	Frag = 2,
	Geom = 4,
};

class ShaderLib : public Singleton<ShaderLib>, public IScriptable
{
	friend class GlobalMaterial;
	SINGLETON_CTOR(ShaderLib)
	std::map<std::string, Shader*> shaders;
public:
	Shader* Get(std::string& name);
	Shader* Load(std::string&& name, unsigned int shaderfiletype = (ShaderFileType::Vert | ShaderFileType::Frag));
};