#pragma once
#include <string>
#include <vector>
#include <TinyGLTF/tiny_gltf.h>

#include "Vertex.h"
#include "Node.h"
#include "Material.h"
#include "Primitive.h"

namespace Atrium {
	class Mesh : public Node{
	public:
		Mesh(const tinygltf::Node& gltfNode, Transform parentTransform,
			const tinygltf::Model& model);
	public:
		std::vector<Primitive> primitives;
	};
}


