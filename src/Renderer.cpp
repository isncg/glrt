#include "../include/Application.h"
#include "../include/Renderer.h"
#include "../utils/utils.h"
#include <algorithm>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#pragma comment(lib, "assimp-vc142-mt.lib")

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
}

void MeshRenderer::Draw()
{
	if (vao <= 0 || indices.size() < 3)
		return;
	Renderer::Draw();
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
	GLASSERT(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, NULL));
	//GLASSERT(glDrawArrays(GL_TRIANGLES, 0, triangleCount * 3));
}

void MeshRenderer::Draw(Shader& shader)
{
	if (vao <= 0 || indices.size() < 3)
		return;
	Renderer::Draw(shader);
	GLASSERT(glBindVertexArray(vao));	
	GLASSERT(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, NULL));
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

size_t Mesh::GetBufferSize()
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

#define MERGE_COMPONENT(c) this->c.insert(this->c.end(), other.c.begin(), other.c.end())

void Mesh::Merge(Mesh& other)
{
	auto indicesoffset = this->vertices.size();
	MERGE_COMPONENT(vertices);
	MERGE_COMPONENT(normals);
	MERGE_COMPONENT(tangents);
	MERGE_COMPONENT(uv);
	MERGE_COMPONENT(uv2);
	MERGE_COMPONENT(uv3);
	MERGE_COMPONENT(uv4);
	MERGE_COMPONENT(uv5);
	MERGE_COMPONENT(uv6);
	MERGE_COMPONENT(uv7);
	MERGE_COMPONENT(uv8);
	MERGE_COMPONENT(colors);
	MERGE_COMPONENT(colors32);

	for (auto i : other.triangles)
	{
		this->triangles.push_back((unsigned int)(indicesoffset + i));
	}
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

size_t CanvasMesh::GetBufferSize()
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
	matInfos.clear();
	meshDict.clear();
}

bool LoadMesh(Mesh* output, aiMesh* input)
{
	output->pMaterialInfo = new MeshMaterialInfo();
	output->pMaterialInfo->materialid = input->mMaterialIndex;
	for (int i = 0; i < (int)input->mNumVertices; i++)
	{
		if (input->mVertices != NULL)
			output->vertices.push_back(Vector3{ input->mVertices[i].x, input->mVertices[i].y, input->mVertices[i].z });

		if (input->mNormals != NULL)
			output->normals.push_back(Vector3{ input->mNormals[i].x, input->mNormals[i].y, input->mNormals[i].z });

		if (input->mTangents != NULL)
			output->tangents.push_back(Vector4{ input->mTangents[i].x, input->mTangents[i].y, input->mTangents[i].z, 1.0f });

		std::vector<Vector2>* uvset[8]{
			&output->uv,
			&output->uv2,
			&output->uv3,
			&output->uv4,
			&output->uv5,
			&output->uv6,
			&output->uv7,
			&output->uv8,
		};
		for (int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS && j < std::end(uvset) - std::begin(uvset); j++)
		{
			if (input->mTextureCoords[j] != NULL)
			{
				auto& uv = input->mTextureCoords[j][i];
				uvset[j]->push_back(Vector2{ uv.x, uv.y });
			}
		}

		std::vector<std::vector<Color>*> colorset
		{
			&output->colors
		};
		for (int j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS && j < colorset.size(); j++)
		{
			if (input->mColors[j] != NULL)
			{
				auto& color = input->mColors[j][i];
				colorset[j]->push_back(Color{ color.r, color.g, color.b, color.a });
			}
		}
	}
	for (unsigned int f = 0; f < input->mNumFaces; f++)
	{
		aiFace& face = input->mFaces[f];
		for (unsigned int i = 0; i < face.mNumIndices - 2; i++)
		{
			output->triangles.push_back(face.mIndices[i]);
			output->triangles.push_back(face.mIndices[i + 1]);
			output->triangles.push_back(face.mIndices[i + 2]);
		}
	}
	return true;
}


bool Model::Load(std::string filename)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!scene)
	{
		log(importer.GetErrorString(), LogLevel::Error);
		return false;
	}
	// Now we can access the file's contents. 

	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		Mesh mesh;
		mesh.pMaterialInfo = new MeshMaterialInfo{ (int)i, std::string(scene->mMaterials[i]->GetName().C_Str()) };
		mergedMesh.push_back(mesh);
		matInfos.push_back(mesh.pMaterialInfo);
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		Mesh mesh;
		LoadMesh(&mesh, scene->mMeshes[i]);
		std::cout << "Loaded mesh " << filename << "[" << i << "] vertices:" << mesh.vertices.size() << " indices:" << mesh.triangles.size() << std::endl;
		if (mesh.vertices.size() > 0 && mesh.triangles.size() > 0)
		{
			//DumpMesh(&mesh);
			meshCollection.push_back(mesh);
			mergedMesh[mesh.pMaterialInfo->materialid].Merge(mesh);
		}
		else
		{
			std::cout << "Mesh invalid" << std::endl;
		}
	}

	//DoTheSceneProcessing(scene);
	// We're done. Everything will be cleaned up by the importer destructor
	return true;
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

void CanvasMesh::MergeBatch(CanvasMesh& mesh)
{
	vertices.insert(vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
	uv.insert(uv.end(), mesh.uv.begin(), mesh.uv.end());
	uv2.insert(uv2.end(), mesh.uv2.begin(), mesh.uv2.end());
}

void CanvasMesh::MergeBatch(CanvasRect&& rect, Vector2 halfsize)
{
	CanvasMesh mesh;
	mesh.Set(std::move(rect), halfsize);
	MergeBatch(mesh);
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
