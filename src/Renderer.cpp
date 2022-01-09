#include "../include/Application.h"
#include "../include/Mesh.h"
#include "../include/Renderer.h"
#include "../utils/utils.h"
#include <algorithm>
#include <set>

bool MeshRenderer::ValidateMesh(Mesh* pMesh)
{
	size_t vertexCount = 0;
	if (nullptr == pMesh)
		return false;
	vertexCount = pMesh->vertices.size();
	if (vertexCount == 0)
		return false;
	if (pMesh->normals.size() > 0 && pMesh->normals.size() != vertexCount) return false;
	if (pMesh->tangents.size() > 0 && pMesh->tangents.size() != vertexCount) return false;
	if (pMesh->normals.size() > 0 && pMesh->normals.size() != vertexCount) return false;
	if (pMesh->normals.size() > 0 && pMesh->normals.size() != vertexCount) return false;
	return true;
}

class UndirectionalGraph
{
	std::map<unsigned int, std::set<unsigned int>> data;
public:
	bool Add(unsigned int a, unsigned b)
	{
		auto connectionSetA = data.find(a);
		auto connectionSetB = data.find(b);
		if (connectionSetA != data.end() && connectionSetA->second.find(b) != connectionSetA->second.end())
			return false;

		if (connectionSetB != data.end() && connectionSetB->second.find(a) != connectionSetB->second.end())
			return false;

		if (connectionSetA == data.end())
			data[a] = { b };
		else
			connectionSetA->second.insert(b);
		if (connectionSetB == data.end())
			data[b] = { a };
		else
			connectionSetB->second.insert(a);
		return true;
	}
};

void MeshRenderer::Set(Mesh* pMesh)
{
	GLASSERT(glGenVertexArrays(1, &vao));
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	GLASSERT(glBindVertexArray(vao));
	GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	auto bufsize = pMesh->GetBufferSize();
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
	vbb.append_attribute_data(pMesh->colors);
	vbb.append_attribute_data(pMesh->colors32);

	vbb.build();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vectorsizeof(pMesh->triangles), &pMesh->triangles.front(), GL_STATIC_DRAW);

	delete[] buffer;
	GLASSERT(glBindVertexArray(0));
	GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	UndirectionalGraph graph;
	for (int ti = 0; ti + 2 < pMesh->triangles.size(); ti += 3)
	{
		auto a = pMesh->triangles[ti];
		auto b = pMesh->triangles[ti+1];
		auto c = pMesh->triangles[ti+2];
		if (graph.Add(a, b))
		{
			edges.push_back(a);
			edges.push_back(b);
		}
		if (graph.Add(a, c))
		{
			edges.push_back(a);
			edges.push_back(c);
		}
		if (graph.Add(c, b))
		{
			edges.push_back(c);
			edges.push_back(b);
		}
	}
}

void MeshRenderer::Draw()
{
	if (vao <= 0 || indices.size() < 3)
		return;
	Renderer::Draw();
	GLASSERT(glBindVertexArray(vao));
	if (isDrawingWireframe)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		isDrawingWireframe = false;
	}
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
	//UseMaterial();
	GLASSERT(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, NULL));
	//GLASSERT(glDrawArrays(GL_TRIANGLES, 0, triangleCount * 3));
}

void MeshRenderer::Draw(Shader& shader)
{
	if (vao <= 0 || indices.size() < 3)
		return;
	Renderer::Draw(shader);
	GLASSERT(glBindVertexArray(vao));	
	if (isDrawingWireframe)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		isDrawingWireframe = false;
	}
	GLASSERT(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, NULL));
}

void MeshRenderer::DrawWireframe()
{
	if (vao <= 0 || edges.size() < 2)
		return;
	Renderer::Draw();
	GLASSERT(glBindVertexArray(vao));
	if (!isDrawingWireframe)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		isDrawingWireframe = true;
	}
	GLASSERT(glDrawElements(GL_LINES, (GLsizei)edges.size(), GL_UNSIGNED_INT, &edges.front()));
}

void MeshRenderer::DrawWireframe(Shader& shader)
{
	if (vao <= 0 || edges.size() < 2)
		return;
	Renderer::Draw(shader);
	GLASSERT(glBindVertexArray(vao));
	if (!isDrawingWireframe)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		isDrawingWireframe = true;
	}
	GLASSERT(glDrawElements(GL_LINES, (GLsizei)edges.size(), GL_UNSIGNED_INT, &edges.front()));
}

std::vector<MeshRenderer*> MeshRenderer::CreateRenderers(std::vector<Mesh> meshset)
{
	std::vector<MeshRenderer*> result;
	//if (nullptr != materialLibrary) 
	//{
		for (auto& mesh : meshset)
		{
			if (mesh.vertices.size() <= 0 || nullptr == mesh.pMaterialInfo)
				continue;
			auto& matlib = MaterialLib::Instance();
			auto pmat = matlib.Get(mesh.pMaterialInfo->name);
			
			if (nullptr == pmat)
				pmat = matlib.Get("default");
			if (nullptr == pmat)
				pmat = matlib.Get("DefaultMaterial");
			if (nullptr == pmat)
				continue;
			MeshRenderer* pmr = new MeshRenderer();
			pmr->material = pmat;
			pmr->Set(&mesh);
			result.push_back(pmr);
		}
		std::sort(result.begin(), result.end(),
			[](const MeshRenderer* a, const MeshRenderer* b)
			{
				return a->material->renderingOrder < b->material->renderingOrder;
			});
	//}
	/*else
	{
		for (auto& mesh : meshset)
		{
			if (mesh.vertices.size() <= 0 || nullptr == mesh.pMaterialInfo)
				continue;
			MeshRenderer* pmr = new MeshRenderer();
			pmr->Set(&mesh);
			result.push_back(pmr);
		}
	}*/

	return result;
}


void CanvasRenderer::Set(CanvasMesh* pMesh)
{
	GLASSERT(glGenVertexArrays(1, &vao));
	GLASSERT(glGenBuffers(1, &vbo));
	GLASSERT(glGenBuffers(1, &ibo));

	GLASSERT(glBindVertexArray(vao));
	GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	auto bufsize = pMesh->GetBufferSize();
	for (auto i = 0; i < pMesh->vertices.size() - 3; i += 4)
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

	GLASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, vectorsizeof(indices), &indices.front(), GL_STATIC_DRAW));

	delete[] buffer;
	GLASSERT(glBindVertexArray(0));
	GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void CanvasRenderer::SetFullScreen()
{
	CanvasMesh mesh;
	mesh.vertices.push_back({ -1,-1 });
	mesh.vertices.push_back({ 1,-1 });
	mesh.vertices.push_back({ 1,1 });
	mesh.vertices.push_back({ -1,1 });

	mesh.uv.push_back({ 0,0 });
	mesh.uv.push_back({ 1,0 });
	mesh.uv.push_back({ 1,1 });
	mesh.uv.push_back({ 0,1 });


	Set(&mesh);
}

void CanvasRenderer::Draw()
{
	if (vao <= 0 || indices.size() < 3)
		return;
	Renderer::Draw();
	//UseMaterial();
	GLASSERT(glBindVertexArray(vao));
	GLASSERT(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, NULL));
}


void Model::Clear()
{
	for (auto& i : meshCollection)
		i.Clear();
	meshCollection.clear();
	matInfos.clear();
	meshDict.clear();
}



void Renderer::Draw()
{
	if (nullptr != material)
		material->Use();
}

void Renderer::Draw(Shader& shader)
{
	shader.Use();
}

void Renderer::DrawWireframe()
{
	if (nullptr != material)
		material->Use();
}

void Renderer::DrawWireframe(Shader& shader)
{
	shader.Use();
}
