#pragma once
#include <string>
#include <vector>
#include <TinyGLTF/tiny_gltf.h>

#include "Vertex.h"
#include "Node.h"
#include "Material.h"

namespace Atrium {
	class Mesh : public Node{
	public:
		Mesh(const tinygltf::Node& gltfNode, Transform parentTransform,
			const tinygltf::Model& model);
	private:
		void LoadAttributes(const tinygltf::Primitive& primitive, const tinygltf::Model& model);
		void LoadIndices(const tinygltf::Primitive& primitive, const tinygltf::Model& model);
		void LoadMaterial(const tinygltf::Primitive& primitive, const tinygltf::Model& model);
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		Material material;
	};
}


