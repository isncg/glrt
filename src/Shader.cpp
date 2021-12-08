#include "../include/Application.h"
#include "../include/Shader.h"
#include "../utils/utils.h"
GLuint loadShader(std::string& filename, GLenum shaderType)
{
	auto glsl = string_readf(filename);
	GLuint shader = glCreateShader(shaderType);
	const GLchar* string = glsl.c_str();
	GLint length = glsl.length();
	glAssert("before_loadShader");
	GLASSERT(glShaderSource(shader, 1, &string, &length));
	log(string_format("Compile shader %s %d", filename.c_str(), shaderType).c_str());
	GLASSERT(glCompileShader(shader));
	//https://www.khronos.org/opengl/wiki/Example/GLSL_Shader_Compile_Error_Testing
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		// Provide the infolog in whatever manor you deem best.
		log(string_format("Shader compiling failed '%s'\n%s", filename.c_str(), &errorLog.front()).c_str(), LogLevel::Error);
		// Exit with failure.
		glDeleteShader(shader); // Don't leak the shader.
		return 0;
	}
	return shader;
}


//void Shader::Load(std::string vert, std::string frag)
//{
//	Load(std::move(vert), std::move(frag));
//	/*GLuint vertexShader = loadShader(vert, GL_VERTEX_SHADER);
//	GLuint fragmentShader = loadShader(frag, GL_FRAGMENT_SHADER);
//
//	GLASSERT(program = glCreateProgram());
//	GLASSERT(glAttachShader(program, vertexShader));
//	GLASSERT(glAttachShader(program, fragmentShader));
//
//	GLASSERT(glLinkProgram(program));*/
//}

GLuint ComplieAndLink(std::string& vert, std::string& frag)
{
	GLuint vertexShader = loadShader(vert, GL_VERTEX_SHADER);
	GLuint fragmentShader = loadShader(frag, GL_FRAGMENT_SHADER);

	GLASSERT(GLuint program = glCreateProgram());
	GLASSERT(glAttachShader(program, vertexShader));
	GLASSERT(glAttachShader(program, fragmentShader));
	GLASSERT(glLinkProgram(program));
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return program;
}

void Shader::Load(std::string&& vert, std::string&& frag)
{
	/*GLuint vertexShader = loadShader(vert, GL_VERTEX_SHADER);
	GLuint fragmentShader = loadShader(frag, GL_FRAGMENT_SHADER);

	GLASSERT(program = glCreateProgram());
	GLASSERT(glAttachShader(program, vertexShader));
	GLASSERT(glAttachShader(program, fragmentShader));

	GLASSERT(glLinkProgram(program));*/
	program = ComplieAndLink(vert, frag);

	this->vert = vert;
	this->frag = frag;
	ResourceMonitor::Instance().StopWatch(this);
	ResourceMonitor::Instance().Watch(vert, this);
	ResourceMonitor::Instance().Watch(frag, this);
}

void Shader::Use()
{
	glAssert("before shader Use");
	GLASSERT(glUseProgram(program));
}

void Shader::Set(const char* name, Matrix4x4& value)
{
	Set(name, std::move(value));
}

void Shader::Set(const char* name, Matrix4x4&& value)
{
	auto loc = glGetUniformLocation(program, name);
	if (loc < 0)
		return;
	GLASSERT(glProgramUniformMatrix4fv(program, loc, 1, false, (const GLfloat*)&value));
}

void Shader::Set(const char* name, Vector2& value)
{
	Set(name, std::move(value));
}

void Shader::Set(const char* name, Vector2&& value)
{
	auto loc = glGetUniformLocation(program, name);
	if (loc < 0)
		return;
	GLASSERT(glProgramUniform2fv(program, loc, 1, (const GLfloat*)&value));
}

void Shader::Set(const char* name, Vector3& value)
{
	Set(name, std::move(value));
}

void Shader::Set(const char* name, Vector3&& value)
{
	auto loc = glGetUniformLocation(program, name);
	if (loc < 0)
		return;
	GLASSERT(glProgramUniform3fv(program, loc, 1, (const GLfloat*)&value));
}

void Shader::Set(const char* name, float value)
{
	GLASSERT(auto loc = glGetUniformLocation(program, name));
	if (loc < 0)
		return;
	GLASSERT(glProgramUniform1f(program, loc, value));
}

void Shader::Set(const char* name, Texture& texture)
{
	GLASSERT(auto loc = glGetUniformLocation(program, name));
	if (loc < 0)
		return;
	if (texture.handle == 0)
	{
		GLASSERT(texture.handle = glGetTextureHandleARB(texture.id));
	}
	if (!glIsTextureHandleResidentARB(texture.handle))
		GLASSERT(glMakeTextureHandleResidentARB(texture.handle));
	GLASSERT(glProgramUniformHandleui64ARB(program, loc, texture.handle));
}


void Shader::OnResourceUpdated()
{
	GLuint newProgram = ComplieAndLink(vert, frag);
	if (newProgram != 0)
	{
		glDeleteProgram(program);
		program = newProgram;
		log(string_format("Shader reload finished %s,%s", vert.c_str(), frag.c_str()).c_str());
	}
	else
	{
		log(string_format("Shader reload fail %s,%s", vert.c_str(), frag.c_str()).c_str());
	}
}

Shader::~Shader()
{
	ResourceMonitor::Instance().StopWatch(this);
}