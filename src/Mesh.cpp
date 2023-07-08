#include "Mesh.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>

using namespace Atrium;


Mesh::Mesh(const tinygltf::Node& gltfNode, Transform parentTransform, const tinygltf::Model& model) : Node(gltfNode, parentTransform) {
	if (gltfNode.mesh < 0) {
		std::cout << "Tried to create Mesh node from non-mesh gltf node '" << gltfNode.name << "'!" << std::endl;
		exit(-1);
	}
	type = NodeType::Mesh;

	for (const tinygltf::Primitive primitive : model.meshes[gltfNode.mesh].primitives) 
		primitives.push_back(Primitive(primitive, model));
}
