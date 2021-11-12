#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Renderer.h"
class IMaterialParam
{
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
	MaterialParam(std::string& name, T& value);
	void SetUniform(GLuint program, GLint location) override;
};

class Material
{
	GLuint program;
	std::map<GLint, IMaterialParam*> params;
	std::map<GLint, IMaterialParam*> managedParams;
public:
	Material();
	Material(const Shader& shader);
	void Set(const Shader& shader);
	void Set(IMaterialParam* param);
	template <typename T>
	void Set(std::string name, T& value);
	void Set(std::string name, float value);
	//void Set(std::string name, int value);
	void Use();
};

template<typename T>
inline MaterialParam<T>::MaterialParam(std::string& name, T& value)
{
	this->name = name;
	this->value = value;
}

template<typename T>
inline void MaterialParam<T>::SetUniform(GLuint program, GLint location)
{
	_SetUniform(value, program, location);
}

template<typename T>
inline void Material::Set(std::string name, T& value)
{
	int location = glGetUniformLocation(program, name.c_str());
	if (location < 0)
		return;
	params.erase(location);
	auto it = managedParams.find(location);
	if (it != managedParams.end())
		delete it->second;
	MaterialParam<T>* pParam = new MaterialParam<T>(name, value);
	managedParams[location] = pParam;
}
