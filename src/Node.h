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
	enum class Space {
		Global,
		Local
	};
	class Node {
	public:
		Node(const tinygltf::Node& gltfNode, glm::mat4 parentWorldTransform);
		std::string ToString(unsigned int depth = 0) const;
	
		glm::mat4 GetTransform(Space space) const;
		void Translate(glm::vec3 translation, Space space = Space::Global);
		void Scale(glm::vec3 factor, Space space = Space::Global);
		void Rotate(float angleInDegrees, glm::vec3 axis, Space space = Space::Global);
		void UpdateChildrenTransforms();
	private:
		void LoadLocalTransforms(const tinygltf::Node& gltfNode);
	public:
		std::string name;
		NodeType type;
		std::vector<Node*> children;
	private:
		glm::mat4 localRotation;
		glm::mat4 localScale;
		glm::mat4 localTranslation;
		glm::mat4 parentWorldTransform;
	};
}


