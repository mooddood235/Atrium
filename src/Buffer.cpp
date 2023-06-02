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

	for (Vertex vertex : mesh->vertices) {
		vertex.position = modelMatrix * glm::vec4(vertex.position, 1.0f);
		vertices.push_back(vertex);
	}
	for (const Node* node : mesh->children) 
		if (node->GetType() == NodeType::Mesh) LoadMeshes((const Mesh*)node);
}
void Buffer::GenerateSSBOs() {
	/*struct GPUVertex {
		glm::vec3 p;
		float pad0;
	};
	struct GPUTriangle {
		unsigned int i0, i1, i2;
		float pad0;
	};
	std::vector<GPUVertex> GPUVertices = std::vector<GPUVertex>(vertices.size());
	for (const Vertex& vertex : vertices) GPUVertices.push_back(GPUVertex{ vertex.position });
	
	std::vector<GPUTriangle> GPUTriangles = std::vector<GPUTriangle>(triangles.size());
	for (const Triangle& triangle : triangles) GPUTriangles.push_back(GPUTriangle{ triangle.index0, triangle.index1, triangle.index2 });*/

	glGenBuffers(1, &verticesSSBO);
	glGenBuffers(1, &trianglesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, verticesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 * vertices.size(), NULL, GL_STATIC_DRAW);

	for (unsigned int i = 0; i < vertices.size(); i++) 
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 16, 12, &vertices[i]);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 * triangles.size(), NULL, GL_STATIC_DRAW);

	for (unsigned int i = 0; i < triangles.size(); i++) 
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 16, 12, &triangles[i]);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
