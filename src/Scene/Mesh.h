#pragma once
#include <string>
#include <vector>
#include <TinyGLTF/tiny_gltf.h>

#include "../Data/Vertex.h"
#include "Node.h"
#include "../Data/Material.h"
#include "../Data/Primitive.h"
#include "../Data/Transform.h"

namespace Atrium {
	class Mesh : public Node{
	public:
		Mesh(const tinygltf::Node& gltfNode, Transform parentTransform,
			const tinygltf::Model& model);
	public:
		std::vector<Primitive> primitives;
	};
}


