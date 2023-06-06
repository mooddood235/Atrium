#include "Buffer.h"
#include <iostream>
using namespace Atrium;

Buffer::Buffer() {
	triangles = std::vector<Triangle>();
	vertices = std::vector<Vertex>();
}
Buffer::Buffer(const Scene& scene) {
	LoadMeshes(scene, vertices, triangles);
	GenerateSSBOs();
}
void Buffer::Bind() const{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, verticesSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, trianglesSSBO);
}

void Buffer::GenerateSSBOs() {
	#pragma pack(push, 1)
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
	
	delete GPUVertices;
	delete GPUTriangles;
}
