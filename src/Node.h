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
		Node(const tinygltf::Node& gltfNode, const tinygltf::Node* gltfParentNode);
		std::string ToString(unsigned int depth = 0) const;
	
		glm::mat4 GetTransform(Space space) const;
		void Translate(glm::vec3 translation, Space space = Space::Global);
		void Scale(glm::vec3 factor, Space space = Space::Global);
		void Rotate(float angleInDegrees, glm::vec3 axis, Space space = Space::Global);
	private:
		void LoadTransformations(const tinygltf::Node* gltfNode,
			glm::fquat& quaternion, glm::vec3& scale, glm::vec3& translation);

		glm::mat4 GetParentWorldTransform() const;
		glm::mat3 GetRotationMatrix(Space space) const;
	public:
		std::string name;
		NodeType type;
		std::vector<Node*> children;
	private:
		glm::fquat lQuaternion, gQuaternion;
		glm::vec3 lScale, gScale;
		glm::vec3 lTranslation, gTranslation;
	};
}


