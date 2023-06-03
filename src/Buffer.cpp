#include "Buffer.h"
#include <iostream>
using namespace Atrium;

Buffer::Buffer() {
	triangles = std::vector<Triangle>();
	vertices = std::vector<Vertex>();
}
Buffer::Buffer(const Scene& scene) {
	for (const Node* node : scene.hierarchy) 
		if (node->GetType() == NodeType::Mesh) LoadMeshes((const Mesh*)node);

	GenerateSSBOs();
}
void Buffer::Bind() const{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, verticesSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, trianglesSSBO);
}
void Buffer::LoadMeshes(const Mesh* mesh) {
	for (unsigned int i = 0; i < mesh->indices.size(); i += 3) {
		Triangle triangle(mesh->indices[i], mesh->indices[i + 1], mesh->indices[i + 2]);
		triangles.push_back(triangle + vertices.size());
	}
	glm::mat4 modelMatrix = mesh->GetTransform(Space::Global).GetMatrix();
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(modelMatrix));

	for (Vertex vertex : mesh->vertices) {
		vertex.position = modelMatrix * glm::vec4(vertex.position, 1.0f);
		vertex.normal = glm::normalize(normalMatrix * vertex.normal);
		vertices.push_back(vertex);
	}
	for (const Node* node : mesh->children) 
		if (node->GetType() == NodeType::Mesh) LoadMeshes((const Mesh*)node);
}
void Buffer::GenerateSSBOs() {
	struct GPUVertex {
		glm::vec3 p;
		float pad0;
		glm::vec3 n;
		float pad1;
		GPUVertex(glm::vec3 position, glm::vec3 normal) {
			p = position;
			n = normal;
		}
	};
	struct GPUTriangle {
		unsigned int i0, i1, i2;
		float pad0;
	};
	GPUVertex* GPUVertices = (GPUVertex*)malloc(sizeof(GPUVertex) * vertices.size());
	for (unsigned int i = 0; i < vertices.size(); i++) GPUVertices[i] = GPUVertex(vertices[i].position, vertices[i].normal);
	
	GPUTriangle* GPUTriangles = (GPUTriangle*)malloc(sizeof(GPUTriangle) * triangles.size());
	for (unsigned int i = 0; i < triangles.size(); i++)
		GPUTriangles[i] = GPUTriangle{ triangles[i].index0, triangles[i].index1, triangles[i].index2 };

	glGenBuffers(1, &verticesSSBO);
	glGenBuffers(1, &trianglesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, verticesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUVertex) * vertices.size(), GPUVertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUTriangle) * triangles.size(), GPUTriangles, GL_STATIC_DRAW);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	free(GPUVertices);
	free(GPUTriangles);
}
