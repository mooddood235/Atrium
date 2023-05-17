#pragma once
#include <string>
#include <vector>
#include <TinyGLTF/tiny_gltf.h>

#include "Vertex.h"
#include "Node.h"

namespace Atrium {
	class Mesh : public Node{
	public:
		Mesh(const tinygltf::Node& gltfNode, const tinygltf::Model& model);
	private:
		void LoadAttributes(const tinygltf::Primitive& primitive, const tinygltf::Model& model);
		void LoadIndices(const tinygltf::Primitive& primitive, const tinygltf::Model& model);
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};
}


