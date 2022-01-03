#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <list>
#include <framework.h>
#include "Texture.h"
#include "Shader.h"
#include <functional>
class IMaterialParam;
class Material
{
	friend class MaterialLib;
	Shader* pShader;
	std::list<IMaterialParam*> params;
	std::map<std::string, IMaterialParam*> lookup;
public:
	void OnValidate();
	std::string name;
	int renderingOrder = 0;
	Material();
	Material(Shader* shader);
	void Set(Shader* shader);
	template <typename T>
	void Set(std::string name, T&& value);
	template <typename T>
	void Set(std::string name, T& value);
	void Set(std::string name, float value);
	void Use();
	void OnInspector();
};

class Camera;
class GlobalMaterial: public Singleton<GlobalMaterial>
{
	SINGLETON_CTOR(GlobalMaterial)
	std::map<std::string, IMaterialParam*> paramDict;
public:
	template <typename T>
	void Set(std::string name, T&& value);
	template <typename T>
	void Set(std::string name, T& value);
	void SetMainCamera(Camera* cam);
	void Use();
	void OnInspector();
};

class IMaterialParam
{
	friend class Material;
public:
	int locationCache = -1;
	std::string name;
	void SetUniform(GLuint program);
	virtual void SetUniform(GLuint program, GLint location) = 0;
	virtual IMaterialParam* Clone() = 0;
	virtual void OnInspector() = 0;
	static void _SetUniform(float value, GLuint program, GLint location);
	static void _SetUniform(Vector3& value, GLuint program, GLint location);
	static void _SetUniform(Vector2& value, GLuint program, GLint location);
	static void _SetUniform(Vector4& value, GLuint program, GLint location);
	static void _SetUniform(Color& value, GLuint program, GLint location);
	static void _SetUniform(ColorRGB& value, GLuint program, GLint location);
	static void _SetUniform(Texture& value, GLuint program, GLint location);
	static void _SetUniform(Matrix4x4& value, GLuint program, GLint location);

	static bool _OnInspector(std::string name, float& value);
	static bool _OnInspector(std::string name, Vector3& value);
	static bool _OnInspector(std::string name, Vector2& value);
	static bool _OnInspector(std::string name, Vector4& value);
	static bool _OnInspector(std::string name, Color& value);
	static bool _OnInspector(std::string name, ColorRGB& value);
	static bool _OnInspector(std::string name, Texture& value);
	static bool _OnInspector(std::string name, Matrix4x4& value);
};


template <typename T>
class MaterialParam : public IMaterialParam
{
public:
	T value;
	MaterialParam(std::string& name, T&& value);
	void SetUniform(GLuint program, GLint location) override;
	void OnInspector() override;
	IMaterialParam* Clone() override;
};


template<typename T>
inline MaterialParam<T>::MaterialParam(std::string& name, T&& value)
{
	this->name = name;
	this->value = value;
	this->locationCache = -2;
}


template<typename T>
inline void MaterialParam<T>::SetUniform(GLuint program, GLint location)
{
	_SetUniform(value, program, location);
}

template<typename T>
inline void MaterialParam<T>::OnInspector()
{
	_OnInspector(this->name, this->value);
}

template<typename T>
inline IMaterialParam* MaterialParam<T>::Clone()
{
	return new MaterialParam<T>(*this);
}


template<typename T>
inline void Material::Set(std::string name, T&& value)
{
	auto it = lookup.find(name);
	if (it != lookup.end())
	{
		((MaterialParam<T>*)(it->second))->value = value;
	}
	else
	{
		for (auto& i : params)
		{
			if (i->name == name)
			{
				((MaterialParam<T>*)i)->value = value;
				lookup[name] = i;
				return;
			}
		}
		MaterialParam<T>* pParam = new MaterialParam<T>(name, std::move(value));
		lookup[name] = pParam;
		params.push_back(pParam);
	}
}


template<typename T>
inline void Material::Set(std::string name, T& value)
{
	Set(name, std::move(value));
}


class MaterialLib: public Singleton<MaterialLib>
{
	SINGLETON_CTOR(MaterialLib)
	friend class MaterialLibInspector;
private:
	std::map<std::string, Material*> dict;
public:
	void Add(Shader* pShader, std::string name, std::function<void(Material&)> op);
	void OnShaderUpdated(Shader* pShader);
	Material* Get(std::string name) const;
};


class MaterialLibInspector
{
	Material* curmat = nullptr;
	bool showMatInfo = false;
public:
	void OnInspector();
	static MaterialLibInspector& Default();
};

template<typename T>
inline void GlobalMaterial::Set(std::string name, T&& value)
{	
	auto it = paramDict.find(name);
	if (it != paramDict.end())
	{
		MaterialParam<T>* ptr = (MaterialParam<T>*)it->second;
		ptr->value = value;
		//it->second = value;
	}
	else
	{
		MaterialParam<T>* pParam = new MaterialParam<T>(name, std::move(value));
		paramDict[name] = pParam;
	}
}

template<typename T>
inline void GlobalMaterial::Set(std::string name, T& value)
{
	Set(name, std::move(value));
}
