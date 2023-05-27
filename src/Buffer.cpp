#include "Buffer.h"

using namespace Atrium;

Buffer::Buffer() {
	triangles = std::vector<Triangle>();
	vertices = std::vector<Vertex>();
}
Buffer::Buffer(const Scene& scene) {
	for (const Node* node : scene.hierarchy) 
		if (node->GetType() == NodeType::Mesh) LoadMeshes((const Mesh*)node);
}
void Buffer::LoadMeshes(const Mesh* mesh) {
	for (unsigned int i = 0; i < mesh->indices.size(); i += 3) {
		Triangle triangle(mesh->indices[i], mesh->indices[i + 1], mesh->indices[i + 2]);
		triangles.push_back(triangle + vertices.size());

		vertices.insert(vertices.end(), mesh->vertices.begin(), mesh->vertices.end());
	}
}
