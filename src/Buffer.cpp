#include "Buffer.h"
#include <iostream>
using namespace Atrium;

Buffer::Buffer() {
	triangles = std::vector<Triangle>();
	vertices = std::vector<Vertex>();
}
Buffer::Buffer(const Scene& scene) {
	LoadMeshes(scene, triangles);
	GenerateSSBOs(triangles);
}
void Buffer::Bind() const{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, verticesSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, trianglesSSBO);
}
