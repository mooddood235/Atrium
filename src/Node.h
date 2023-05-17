#pragma once

#include <string>
#include <vector>
#include <TinyGLTF/tiny_gltf.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Atrium {
	enum class NodeType {
		Node,
		Mesh
	};
	class Node {
	public:
		Node(const tinygltf::Node& gltfNode);
		std::string ToString(unsigned int depth = 0) const;
		glm::mat4 GetModelMatrix();
	public:
		std::string name;
		NodeType type;
		std::vector<Node*> children;
	private:
		glm::fquat quaternion;
		glm::vec3 scale;
		glm::vec3 translation;
	};
}


