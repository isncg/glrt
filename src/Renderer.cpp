#include "../include/Application.h"
#include "../include/Renderer.h"
#include "../utils/utils.h"

bool MeshRenderer::ValidateMesh(Mesh* pMesh)
{
	int vertexCount = 0;
	if (nullptr == pMesh)
		return false;
	vertexCount = pMesh->vertices.size();
	if (vertexCount <= 0)
		return false;
	if (pMesh->normals.size() > 0 && pMesh->normals.size() != vertexCount) return false;
	if (pMesh->tangents.size() > 0 && pMesh->tangents.size() != vertexCount) return false;
	if (pMesh->normals.size() > 0 && pMesh->normals.size() != vertexCount) return false;
	if (pMesh->normals.size() > 0 && pMesh->normals.size() != vertexCount) return false;
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
	vbb.append_attribute_data(pMesh->colors);
	vbb.append_attribute_data(pMesh->colors32);

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
	//UseMaterial();
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

void Mesh::Clear()
{
	vertices.clear();
	normals.clear();
	tangents.clear();
	uv.clear();
	uv2.clear();
	uv3.clear();
	uv4.clear();
	uv5.clear();
	uv6.clear();
	uv7.clear();
	uv8.clear();
	colors.clear();
	colors32.clear();
	triangles.clear();
}






void CanvasRenderer::Set(CanvasMesh* pMesh)
{
	GLASSERT(glGenVertexArrays(1, &vao));
	GLASSERT(glGenBuffers(1, &vbo));
	GLASSERT(glGenBuffers(1, &ibo));

	GLASSERT(glBindVertexArray(vao));
	GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	int bufsize = pMesh->GetBufferSize();
	for (int i = 0; i < pMesh->vertices.size() - 3; i += 4)
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
	//UseMaterial();
	GLASSERT(glBindVertexArray(vao));
	GLASSERT(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL));
}

void CanvasMesh::Set(CanvasRect&& rect, Vector2 halfsize)
{
	vertices.clear();
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMin) / halfsize.x, (rect.pos.y + rect.yMin) / halfsize.y });
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMax) / halfsize.x, (rect.pos.y + rect.yMin) / halfsize.y });
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMax) / halfsize.x, (rect.pos.y + rect.yMax) / halfsize.y });
	vertices.push_back(Vector2{ (rect.pos.x + rect.xMin) / halfsize.x, (rect.pos.y + rect.yMax) / halfsize.y });

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

//void Renderer::UseMaterial()
//{
//	material->Use();
//	/*if (passID >= 0)
//	{
//		auto mat = materialPassDict.find(passID);
//		if (mat == materialPassDict.end())
//			return;
//		mat->second->Use();
//	}
//	else if (nullptr != material)
//	{
//		material->Use();
//	}*/
//}

void Model::Clear()
{
	for (auto& i : meshCollection)
		i.Clear();
	meshCollection.clear();
	matNames.clear();
	meshDict.clear();
}

void CanvasRect::SetRect(Vector2 pos, Vector2 size, Vector2 pivot)
{
	this->pos = pos;
	xMin =  - pivot.x * size.x;
	xMax = (1 - pivot.x) * size.x;
	yMin =  - pivot.y * size.y;
	yMax = (1 - pivot.y) * size.y;
}

CanvasRect::CanvasRect()
{
}

CanvasRect::CanvasRect(Vector2 pos, Vector2 size, Vector2 pivot)
{
	SetRect(pos, size, pivot);
}

void tagCanvasMesh::MergeBatch(tagCanvasMesh& mesh)
{
	vertices.insert(vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
	uv.insert(uv.end(), mesh.uv.begin(), mesh.uv.end());
	uv2.insert(uv2.end(), mesh.uv2.begin(), mesh.uv2.end());
}

void tagCanvasMesh::MergeBatch(CanvasRect&& rect, Vector2 halfsize)
{
	CanvasMesh mesh;
	mesh.Set(std::move(rect), halfsize);
	MergeBatch(mesh);
}
