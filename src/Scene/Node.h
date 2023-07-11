#pragma once

#include <string>
#include <vector>
#include <TinyGLTF/tiny_gltf.h>
#include <glm/glm.hpp>
#include "../Data/Transform.h"

namespace Atrium {
	enum class NodeType {
		Node,
		Mesh,
		Camera
	};

	class Node {
	public:
		Node(const tinygltf::Node& gltfNode, Transform parentTransform);
		std::string ToString(unsigned int depth = 0) const;
	
		Transform GetTransform(Space space) const;

		std::string GetName() const;
		NodeType GetType() const;

		void AddChild(Node* node);

		void Translate(glm::vec3 translation, Space space = Space::Global);
		void Scale(glm::vec3 factor, Space space = Space::Global);
		void Rotate(float angleInDegrees, glm::vec3 axis, Space space = Space::Global);
		void UpdateChildrenTransforms();
	private:
		void LoadLocalTransform(const tinygltf::Node& gltfNode);
	public:
		std::vector<Node*> children;
	protected:
		std::string name;
		NodeType type;

		Transform parentTransform;
		Transform localTransform;

		/*glm::mat4 localRotation;
		glm::mat4 localScale;
		glm::mat4 localTranslation;
		glm::mat4 parentWorldTransform;*/
	};
}


