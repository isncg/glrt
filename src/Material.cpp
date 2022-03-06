#include "../include/Material.h"
#include <utils/utils.h>
#include "../include/Camera.h"
#include <imgui/imgui.h>
void IMaterialParam::SetUniform(GLuint program)
{
	if (locationCache == -2)
		locationCache = glGetUniformLocation(program, name.c_str());
	SetUniform(program, locationCache);
}
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

void IMaterialParam::_SetUniform(Color& value, GLuint program, GLint location)
{
	glProgramUniform4fv(program, location, 1, (const GLfloat*)&value);
}

void IMaterialParam::_SetUniform(ColorRGB& value, GLuint program, GLint location)
{
	glProgramUniform3fv(program, location, 1, (const GLfloat*)&value);
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

bool IMaterialParam::_OnInspector(std::string name, float& value)
{
	return ImGui::InputFloat(name.c_str(), &value);
}

bool IMaterialParam::_OnInspector(std::string name, Vector3& value)
{
	return ImGui::InputFloat3(name.c_str(), &value.x);
}

bool IMaterialParam::_OnInspector(std::string name, Vector2& value)
{
	return ImGui::InputFloat2(name.c_str(), &value.x);
}

bool IMaterialParam::_OnInspector(std::string name, Vector4& value)
{
	return ImGui::InputFloat4(name.c_str(), &value.x);
}

bool IMaterialParam::_OnInspector(std::string name, Color& value)
{
	return ImGui::ColorEdit4(name.c_str(), &value.r);
}

bool IMaterialParam::_OnInspector(std::string name, ColorRGB& value)
{
	return ImGui::ColorEdit3(name.c_str(), &value.r);
}

bool IMaterialParam::_OnInspector(std::string name, Texture& value)
{
	if (value.target != GL_TEXTURE_2D)
		return false;
	GLASSERT(ImGui::Text(name.c_str()));
	GLASSERT(ImGui::Image((ImTextureID)value.id, ImVec2{ 256,256 }));
	return false;
}

bool IMaterialParam::_OnInspector(std::string name, Matrix4x4& value)
{
	//TODO
	return false;
}


void Material::OnValidate()
{
	if (nullptr == pShader)
		return;
	for (auto it : params)
	{
		it->locationCache = glGetUniformLocation(pShader->program, it->name.c_str());
	}
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
	/*for (auto& it : params)
	{
		it.second->isDirty = true;
	}*/
	/*for (auto& it : managedParams)
	{
		it.second->isDirty = true;
	}
	if (shader->materialTemplate != this && shader->materialTemplate != nullptr)
	{
		for (auto& it : shader->materialTemplate->managedParams)
		{
			int location = glGetUniformLocation(pShader->program, it.second->name.c_str());
			if (location < 0)
				continue;
			auto it2 = managedParams.find(location);
			if (it2 != managedParams.end())
				delete it2->second;
			IMaterialParam* clone = it.second->Clone();
			clone->isDirty = true;
			managedParams[location] = clone;
		}
	}
	isUniformLocated = true;*/
}

//void Material::Set(IMaterialParam* param)
//{
//	int location = glGetUniformLocation(pShader->program, param->name.c_str());
//	if (location < 0)
//		return;
//	auto it = managedParams.find(location);
//	if (it != managedParams.end())
//		delete it->second;
//	managedParams.erase(location);
//	params[location] = param;
//	param->isDirty = true;
//}

void Material::Set(std::string name, float value)
{
	auto it = lookup.find(name);
	if (it != lookup.end())
	{
		((MaterialParam<float>*)(it->second))->value = value;
	}
	else
	{
		for (auto& i : params)
		{
			if (i->name == name)
			{
				auto ptr = ((MaterialParam<float>*)i);
				ptr->value = value;
				lookup[name] = i;
				return;
			}
		}
		MaterialParam<float>* pParam = new MaterialParam<float>(name, std::move(value));
		lookup[name] = pParam;
		params.push_back(pParam);
	}
}

//void Material::Set(std::string name, int value)
//{
//}

void Material::Use()
{
	glUseProgram(pShader->program);
	for (auto& it : params)
	{
		it->SetUniform(pShader->program);
	}
}

void Material::OnInspector()
{
	ImGui::LabelText("Material", this->name.c_str());
	/*for (auto& it : params)
		it.second->OnInspector();*/
	for (auto& it : params)
		it->OnInspector();
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
	if(op)
		op(*pmat);
}

void MaterialLib::OnShaderUpdated(Shader* pShader)
{
	for (auto& it : dict)
	{
		if (it.second->pShader == pShader)
			it.second->OnValidate();
	}
}

Material* MaterialLib::Get(std::string name) const
{
	auto it = dict.find(name);
	return it == dict.end() ? nullptr : it->second;
}

void MaterialLibInspector::OnInspector()
{
	ImGui::Begin("Material Lib");

	for (auto& it : MaterialLib::Instance().dict)
	{
		if (ImGui::Button(it.first.c_str()))
		{
			curmat = it.second;
			showMatInfo = true;
		}
	}

	ImGui::End();
	if (nullptr != curmat && showMatInfo)
	{
		ImGui::Begin("Material Info", &showMatInfo);
		curmat->OnInspector();
		ImGui::End();
	}
}

MaterialLibInspector& MaterialLibInspector::Default()
{
	static MaterialLibInspector obj;
	return obj;
}

void GlobalMaterial::SetMainCamera(Camera* cam)
{
	auto mat = cam->GetMatrix();
	Set("g_cam", mat);
	Set("g_camview", cam->mat_view);
	Set("g_camproj", cam->mat_proj);
}

void GlobalMaterial::Use()
{
	for (auto& shaderDictKV : ShaderLib::Instance().shaders)
	{
		auto program = shaderDictKV.second->program;
		for (auto& param : params)
		{
			glAssert("for (auto& paramKV : paramDict) begin");
			GLASSERT(GLint location = glGetUniformLocation(program, param->name.c_str()));
			if (location < 0)
				continue;
			GLASSERT(param->SetUniform(program, location));
		}
	}
}

void GlobalMaterial::OnInspector()
{
	ImGui::Begin("Global Material");

	for (auto& it : params)
	{
		it->OnInspector();
	}
	ImGui::End();
}
