#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "framework.h"
#include "Texture.h"

class Shader
{
	friend class Material;
	Material* lastMaterial = NULL;
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


class IMaterialParam
{
	friend class Material;
protected:
	bool isDirty = true;
public:
	std::string name;
	virtual void SetUniform(GLuint program, GLint location) = 0;

	static void _SetUniform(float value, GLuint program, GLint location);
	static void _SetUniform(Vector3& value, GLuint program, GLint location);
	static void _SetUniform(Vector2& value, GLuint program, GLint location);
	static void _SetUniform(Texture& value, GLuint program, GLint location);
	static void _SetUniform(Matrix4x4& value, GLuint program, GLint location);
};

template <typename T>
class MaterialParam : public IMaterialParam
{
public:
	T value;
	MaterialParam(std::string& name, T&& value);
	void SetUniform(GLuint program, GLint location) override;
};

class Material
{
	//GLuint program;
	Shader* pShader;
	std::map<GLint, IMaterialParam*> params;
	std::map<GLint, IMaterialParam*> managedParams;
public:
	Material();
	Material(Shader* shader);
	void Set(Shader* shader);
	void Set(IMaterialParam* param);
	template <typename T>
	void Set(std::string name, T&& value);
	template <typename T>
	void Set(std::string name, T& value);
	void Set(std::string name, float value);
	//void Set(std::string name, int value);
	void Use();
};

template<typename T>
inline MaterialParam<T>::MaterialParam(std::string& name, T&& value)
{
	this->name = name;
	this->value = value;
	this->isDirty = true;
}

template<typename T>
inline void MaterialParam<T>::SetUniform(GLuint program, GLint location)
{
	_SetUniform(value, program, location);
	isDirty = false;
}

template<typename T>
inline void Material::Set(std::string name, T&& value)
{
	int location = glGetUniformLocation(pShader->program, name.c_str());
	if (location < 0)
		return;
	params.erase(location);
	auto it = managedParams.find(location);
	if (it != managedParams.end())
		delete it->second;
	MaterialParam<T>* pParam = new MaterialParam<T>(name, std::move(value));
	managedParams[location] = pParam;
	pParam->isDirty = true;
}

template<typename T>
inline void Material::Set(std::string name, T& value)
{
	Set(name, std::move(value));
}
