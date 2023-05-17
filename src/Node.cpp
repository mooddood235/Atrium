#include "Node.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace Atrium;

Node::Node(const tinygltf::Node& gltfNode) {
	name = gltfNode.name;
	type = NodeType::Node;

	children = std::vector<Node*>();

	glm::vec4 rotation = gltfNode.rotation.size() != 0 
		? glm::vec4(gltfNode.rotation[0], gltfNode.rotation[1], gltfNode.rotation[2], gltfNode.rotation[3])
		: glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	quaternion = glm::fquat(rotation.w, rotation.x, rotation.y, rotation.z);

	scale = gltfNode.scale.size() != 0 
		? glm::vec3(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]) 
		: glm::vec3(1.0f);
	translation = gltfNode.translation.size() != 0 
		? glm::vec3(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]) 
		: glm::vec3(0.0f);
}
glm::mat4 Node::GetModelMatrix() {
	glm::mat4 rotationMatrix = glm::toMat4(quaternion);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);

	return translationMatrix * scaleMatrix * rotationMatrix;
}
std::string Node::ToString(unsigned int depth) const{
	std::string str = "-" + name;

	if (type == NodeType::Node) str += " (Node)";
	else if (type == NodeType::Mesh) str += " (Mesh)";

	std::string depthStr;
	depth++;
	for (unsigned int i = 0; i < depth; i++) depthStr += " ";

	for (const Node* child : children) str += "\n" + depthStr + child->ToString(depth);
	return str;
}