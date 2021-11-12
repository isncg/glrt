#include "Material.h"
#include "utils/glhelpers.h"
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

Material::Material(const Shader& shader)
{
	Set(shader);
}

void Material::Set(const Shader& shader)
{
	program = shader.program;
}

void Material::Set(IMaterialParam* param)
{
	int location = glGetUniformLocation(program, param->name.c_str());
	if (location < 0)
		return;
	auto it = managedParams.find(location);
	if (it != managedParams.end())
		delete it->second;
	managedParams.erase(location);
	params[location] = param;
}

void Material::Set(std::string name, float value)
{
	int location = glGetUniformLocation(program, name.c_str());
	if (location < 0)
		return;
	params.erase(location);
	auto it = managedParams.find(location);
	if (it != managedParams.end())
		delete it->second;
	MaterialParam<float>* pParam = new MaterialParam<float>(name, value);
	managedParams[location] = pParam;
}

//void Material::Set(std::string name, int value)
//{
//}

void Material::Use()
{
	glUseProgram(program);
	for (auto& it : params)
	{
		it.second->SetUniform(program, it.first);
	}
	for (auto& it : managedParams)
	{
		it.second->SetUniform(program, it.first);
	}
}
