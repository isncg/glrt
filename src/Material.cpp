#include "../include/Material.h"
#include "../utils/utils.h"

void IMaterialParam::_SetUniform(float value, GLuint program, GLint location)
{
	glProgramUniform1f(program, location, value);
}
void IMaterialParam::_SetUniform(Vector3& value, GLuint program, GLint location)
{
	glProgramUniform3fv(program, location, 1, (const GLfloat*)&value);
}

void IMaterialParam::_SetUniform(Vector2& value, GLuint program, GLint location)
{
	glProgramUniform2fv(program, location, 1, (const GLfloat*)&value);
}

void IMaterialParam::_SetUniform(Vector4& value, GLuint program, GLint location)
{
	glProgramUniform4fv(program, location, 1, (const GLfloat*)&value);
}

void IMaterialParam::_SetUniform(Texture& value, GLuint program, GLint location)
{
	if (value.handle == 0)
	{
		GLASSERT(value.handle = glGetTextureHandleARB(value.id));
	}
	if (!glIsTextureHandleResidentARB(value.handle))
		GLASSERT(glMakeTextureHandleResidentARB(value.handle));
	GLASSERT(glProgramUniformHandleui64ARB(program, location, value.handle));
}

void IMaterialParam::_SetUniform(Matrix4x4& value, GLuint program, GLint location)
{
	GLASSERT(glProgramUniformMatrix4fv(program, location, 1, false, (const GLfloat*)&value));
}


Material::Material()
{
}

Material::Material(Shader* shader)
{
	Set(shader);
}

void Material::Set(Shader* shader)
{
	pShader = shader;
	for (auto& it : params)
	{
		it.second->isDirty = true;
	}
	for (auto& it : managedParams)
	{
		it.second->isDirty = true;
	}
}

void Material::Set(IMaterialParam* param)
{
	int location = glGetUniformLocation(pShader->program, param->name.c_str());
	if (location < 0)
		return;
	auto it = managedParams.find(location);
	if (it != managedParams.end())
		delete it->second;
	managedParams.erase(location);
	params[location] = param;
	param->isDirty = true;
}

void Material::Set(std::string name, float value)
{
	int location = glGetUniformLocation(pShader->program, name.c_str());
	if (location < 0)
		return;
	params.erase(location);
	auto it = managedParams.find(location);
	if (it != managedParams.end())
		delete it->second;
	MaterialParam<float>* pParam = new MaterialParam<float>(name, std::move(value));
	managedParams[location] = pParam;
}

//void Material::Set(std::string name, int value)
//{
//}

void Material::Use()
{
	glUseProgram(pShader->program);
	if (pShader->lastMaterial == this)
	{
		for (auto& it : params)
			if (it.second->isDirty)
				it.second->SetUniform(pShader->program, it.first);
		for (auto& it : managedParams)
			if (it.second->isDirty)
				it.second->SetUniform(pShader->program, it.first);
	}
	else
	{
		for (auto& it : params)
			it.second->SetUniform(pShader->program, it.first);
		for (auto& it : managedParams)
			it.second->SetUniform(pShader->program, it.first);
	}
	pShader->lastMaterial = this;
}

void MaterialLib::Add(Shader* pShader, std::string name, std::function<void(Material&)> op)
{
	Material* pmat = nullptr;
	auto it = dict.find(name);
	if (it == dict.end())
	{
		pmat = new Material(pShader);
		pmat->name = name;
		dict[name] = pmat;
	}
	else
		pmat = it->second;
	op(*pmat);
}

Material* MaterialLib::Get(std::string name) const
{
	auto it = dict.find(name);
	return it == dict.end() ? nullptr : it->second;
}
