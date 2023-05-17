#pragma once

#include <string>
#include <vector>
#include <TinyGLTF/tiny_gltf.h>
#include <glm/glm.hpp>

namespace Atrium {
	enum class NodeType {
		Node,
		Mesh
	};
	class Node {
	public:
		Node(const tinygltf::Node& gltfNode);
		std::string ToString(unsigned int depth = 0) const;
	public:
		std::string name;
		NodeType type;
		std::vector<Node*> children;
	private:
		glm::vec4 rotation;
		glm::vec3 scale;
		glm::vec3 translation;
	};
}


