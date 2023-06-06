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
GPUVertex::GPUVertex() {}
GPUVertex::GPUVertex(const Vertex& vertex){
	p = vertex.position;
	n = vertex.normal;
}
