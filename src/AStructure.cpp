#include "AStructure.h"

using namespace Atrium;

void AStructure::LoadMeshes(const Scene& scene, std::vector<Vertex>& vertices, std::vector<Triangle>& triangles){
	for (const Node* node : scene.hierarchy)
		if (node->GetType() == NodeType::Mesh) 
			LoadMeshesHelper((const Mesh*)node, vertices, triangles);
}



void AStructure::LoadMeshesHelper(const Mesh* mesh, std::vector<Vertex>& vertices, std::vector<Triangle>& triangles){
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
		if (node->GetType() == NodeType::Mesh) 
			LoadMeshesHelper((const Mesh*)node, vertices, triangles);
}
void AStructure::GenerateSSBOs(const std::vector<Triangle>& triangles){
	#pragma pack(push, 1)
	struct GPUVertex {
		glm::vec3 p;
		float pad0;
		glm::vec3 n;
		float pad1;
		GPUVertex() {}
		GPUVertex(const Vertex& vertex) {
			p = vertex.position;
			n = vertex.normal;
		}
	};
	struct GPUTriangle {
		unsigned int i0, i1, i2;
		float pad0;
		GPUTriangle() {}
		GPUTriangle(const Triangle& triangle) {
			i0 = triangle.index0; i1 = triangle.index1; i2 = triangle.index2;
		}
	};
	#pragma pack(pop)

	GPUVertex* GPUVertices = new GPUVertex[vertices.size()];
	for (unsigned int i = 0; i < vertices.size(); i++) GPUVertices[i] = GPUVertex(vertices[i]);

	GPUTriangle* GPUTriangles = new GPUTriangle[triangles.size()];
	for (unsigned int i = 0; i < triangles.size(); i++) GPUTriangles[i] = GPUTriangle(triangles[i]);

	glGenBuffers(1, &verticesSSBO);
	glGenBuffers(1, &trianglesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, verticesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUVertex) * vertices.size(), GPUVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUTriangle) * triangles.size(), GPUTriangles, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] GPUVertices;
	delete[] GPUTriangles;
}