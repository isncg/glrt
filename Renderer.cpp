#include "Renderer.h"
#include "utils/vertex_buffer_builder.h"
#include "utils/stdhelpers.h"
#include "utils/log.h"
bool MeshRenderer::ValidateMesh(Mesh* pMesh)
{
	int vertexCount = 0;
	if (nullptr == pMesh)
		return false;
	vertexCount = pMesh->vertices.size();
	if (vertexCount <= 0)
		return false;
	if (pMesh->normals.size()  > 0  && pMesh->normals.size() != vertexCount) return false;
	if (pMesh->tangents.size() > 0  && pMesh->tangents.size()!= vertexCount) return false;
	if (pMesh->normals.size()  > 0  && pMesh->normals.size() != vertexCount) return false;
	if (pMesh->normals.size()  > 0  && pMesh->normals.size() != vertexCount) return false;
	return true;
}




void MeshRenderer::Set(Mesh* pMesh)
{
	GLASSERT(glGenVertexArrays(1, &vao));
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	GLASSERT(glBindVertexArray(vao));
	GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	int bufsize = pMesh->GetBufferSize();
	indices.assign(pMesh->triangles.begin(), pMesh->triangles.end());
	triangleCount = indices.size() / 3;
	int8_t* buffer = new int8_t[bufsize];
	

	vertex_buffer_builder vbb(buffer, bufsize);
	vbb.append_attribute_data(pMesh->vertices);
	vbb.append_attribute_data(pMesh->normals);
	vbb.append_attribute_data(pMesh->tangents);
	vbb.append_attribute_data(pMesh->uv);
	vbb.append_attribute_data(pMesh->uv2);
	vbb.append_attribute_data(pMesh->uv3);
	vbb.append_attribute_data(pMesh->uv4);
	vbb.append_attribute_data(pMesh->uv5);
	vbb.append_attribute_data(pMesh->uv6);
	vbb.append_attribute_data(pMesh->uv7);
	vbb.append_attribute_data(pMesh->uv8);

	vbb.build();
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vectorsizeof(pMesh->triangles), &pMesh->triangles.front(), GL_STATIC_DRAW);

	delete[] buffer;
	GLASSERT(glBindVertexArray(0));
	GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshRenderer::Draw()
{
	if (vao <= 0 || indices.size() < 3)
		return;
	GLASSERT(glBindVertexArray(vao));

/********************************************************************* glDrawElements *********************************************************************
https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawElements.xhtml
mode
Specifies what kind of primitives to render. Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY and GL_PATCHES are accepted.

count
Specifies the number of elements to be rendered.

type
Specifies the type of the values in indices. Must be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT.

indices
Specifies a pointer to the location where the indices are stored.
********************************************************************** glDrawElements *********************************************************************/

	//GLASSERT(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, &indices.front()));


/********************************************************************* glDrawArrays *********************************************************************
https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawArrays.xhtml
mode
Specifies what kind of primitives to render. Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY and GL_PATCHES are accepted.

first
Specifies the starting index in the enabled arrays.

count
Specifies the number of indices to be rendered.
********************************************************************** glDrawArrays ********************************************************************/
	GLASSERT(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL));
	//GLASSERT(glDrawArrays(GL_TRIANGLES, 0, triangleCount * 3));
}

int Mesh::GetBufferSize()
{
	return
		vectorsizeof(vertices) +
		vectorsizeof(normals) +
		vectorsizeof(tangents) +
		vectorsizeof(uv) +
		vectorsizeof(uv2) +
		vectorsizeof(uv3) +
		vectorsizeof(uv4) +
		vectorsizeof(uv5) +
		vectorsizeof(uv6) +
		vectorsizeof(uv7) +
		vectorsizeof(uv8) +
		vectorsizeof(colors) +
		vectorsizeof(colors32);
}


GLuint loadShader(const char* filename, GLenum shaderType)
{
	auto glsl = string_readf(filename);
	GLuint shader = glCreateShader(shaderType);
	const GLchar* string = glsl.c_str();
	GLint length = glsl.length();
	glAssert("before_loadShader");
	GLASSERT(glShaderSource(shader, 1, &string, &length));
	log(string_format("Compile shader %s %d", filename, shaderType).c_str());
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
		log(string_format("Shader compiling failed '%s'\n%s", filename, &errorLog.front()).c_str(), LogLevel::Error);
		// Exit with failure.
		glDeleteShader(shader); // Don't leak the shader.
		return 0;
	}
	return shader;
}

void Shader::Load(const char* vert, const char* frag)
{
	GLuint vertexShader = loadShader(vert, GL_VERTEX_SHADER);
	GLuint fragmentShader = loadShader(frag, GL_FRAGMENT_SHADER);

	GLASSERT(program = glCreateProgram());
	GLASSERT(glAttachShader(program, vertexShader));
	GLASSERT(glAttachShader(program, fragmentShader));

	GLASSERT(glLinkProgram(program));
}

void Shader::Use()
{
	glAssert("before shader Use");
	GLASSERT(glUseProgram(program));
	textureBindingHelper.ReBind();
}

void Shader::Set(const char* name, Matrix4x4& value)
{
	auto loc = glGetUniformLocation(program, name);
	glUniformMatrix4fv(loc, 1, false, (const GLfloat*)&value);
}

TextureBindingHelper& Shader::NewTextureBinding()
{
	textureBindingHelper.Reset();
	textureBindingHelper.program = program;
	return textureBindingHelper;
}

GLint GetComponentCount(const Color32& tag) { return 4; }

GLenum GetComponentType(const Vector3& tag) { return GL_FLOAT; }
GLenum GetComponentType(const Vector4& tag) { return GL_FLOAT; }
GLenum GetComponentType(const Vector2& tag) { return GL_FLOAT; }
GLenum GetComponentType(const Color32& tag) { return GL_UNSIGNED_BYTE; }

void CanvasRenderer::Set(CanvasMesh* pMesh)
{
	GLASSERT(glGenVertexArrays(1, &vao));
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	GLASSERT(glBindVertexArray(vao));
	GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	int bufsize = pMesh->GetBufferSize();
	for (int i = 0; i < pMesh->vertices.size()-3; i+=4)
	{
		indices.push_back(i); indices.push_back(i + 1); indices.push_back(i + 2);
		indices.push_back(i); indices.push_back(i + 2); indices.push_back(i + 3);
	}
	//indices.assign(pMesh->triangles.begin(), pMesh->triangles.end());
	triangleCount = indices.size() / 3;
	int8_t* buffer = new int8_t[bufsize];


	vertex_buffer_builder vbb(buffer, bufsize);
	vbb.append_attribute_data(pMesh->vertices);
	vbb.append_attribute_data(pMesh->uv);
	vbb.append_attribute_data(pMesh->uv2);

	vbb.build();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vectorsizeof(indices), &indices.front(), GL_STATIC_DRAW);

	delete[] buffer;
	GLASSERT(glBindVertexArray(0));
	GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CanvasRenderer::Draw()
{
	if (vao <= 0 || indices.size() < 3)
		return;
	GLASSERT(glBindVertexArray(vao));
	GLASSERT(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL));
}

void CanvasMesh::Set(CanvasRect& rect, float halfWidth, float halfHeight)
{
	vertices.clear();
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMin) / halfWidth, (rect.pos.y + rect.yMin) / halfHeight });
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMax) / halfWidth, (rect.pos.y + rect.yMin) / halfHeight });
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMax) / halfWidth, (rect.pos.y + rect.yMax) / halfHeight });
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMin) / halfWidth, (rect.pos.y + rect.yMax) / halfHeight });

	uv.clear();
	uv.push_back(Vector2{ 0,0 });
	uv.push_back(Vector2{ 1,0 });
	uv.push_back(Vector2{ 1,1 });
	uv.push_back(Vector2{ 0,1 });

	uv2.clear();
	uv2.push_back(Vector2{ 0,0 });
	uv2.push_back(Vector2{ 1,0 });
	uv2.push_back(Vector2{ 1,1 });
	uv2.push_back(Vector2{ 0,1 });
}

int CanvasMesh::GetBufferSize()
{
	return
		vectorsizeof(vertices) +
		vectorsizeof(uv) +
		vectorsizeof(uv2);
}

TextureBindingHelper& TextureBindingHelper::Bind(const char* name, Texture& texture)
{
	glActiveTexture(GL_TEXTURE0 + textures.size());
	glBindTexture(GL_TEXTURE_2D, texture.id);
	auto location =glGetUniformLocation(program, name);
	glUniform1i(location, textures.size());
	textures.push_back(texture.id);
	names.push_back(name);
	return *this;
}

void TextureBindingHelper::ReBind()
{
	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		auto location = glGetUniformLocation(program, names[i].c_str());
		glUniform1i(location, textures.size());
	}
}

void TextureBindingHelper::Reset()
{
	textures.clear();
	names.clear();
}

void TextureBindingHelper::End()
{
	glActiveTexture(GL_TEXTURE0);
}

void Texture::Load(const char* fname)
{
	glGenTextures(1, &id);
	// TODO: read image file
}
