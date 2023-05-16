#include "Mesh.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>

using namespace Atrium;

Mesh::Mesh(const std::string& name) : Node(name){
	type = NodeType::Mesh;
	vertices = std::vector<Vertex>();
}
Mesh::Mesh(const std::string& name, const tinygltf::Mesh& mesh, const tinygltf::Model& model) : Node(name) {
	type = NodeType::Mesh;
	
	std::vector<glm::vec3> positions;
	
	const tinygltf::Primitive primitive = mesh.primitives[0];
	
	for (const auto& x: primitive.attributes) {
		std::string attributeName = x.first;

		const tinygltf::Accessor& accessor = model.accessors[x.second];
		const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

		size_t totalByteOffset = accessor.byteOffset + bufferView.byteOffset;
		int stride = accessor.ByteStride(bufferView);
			
		const unsigned char* data = buffer.data.data() + totalByteOffset;

		if (attributeName == "POSITION") {
			if (!(accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == GL_FLOAT)) {
				std::cout << "LOAD GLTF ERROR: Only position data that is Vec3s of floats is supported." << std::endl;
				exit(-1);
			}
			positions.resize(accessor.count);

			for (unsigned int i = 0; i < positions.size(); i++)
				positions[i] = *(glm::vec3*)(data + stride * i);
		}
	}
	vertices.resize(positions.size());
	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i] = Vertex(positions[i]);
		//std::cout << positions[i].x << ", " << positions[i].y << ", " << positions[i].z << std::endl;
	}
}