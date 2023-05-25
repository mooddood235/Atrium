#include "Node.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace Atrium;

Node::Node(const tinygltf::Node& gltfNode, glm::mat4 parentWorldTransform) {
	name = gltfNode.name;
	type = NodeType::Node;
	children = std::vector<Node*>();

	this->parentWorldTransform = parentWorldTransform;
	LoadLocalTransforms(gltfNode);
}
void Node::LoadLocalTransforms(const tinygltf::Node& gltfNode) {
	localRotation = gltfNode.rotation.size() == 0 ? glm::mat4(1.0f)
	: glm::toMat4(
		glm::fquat(gltfNode.rotation[3], gltfNode.rotation[0], gltfNode.rotation[1], gltfNode.rotation[2]));
	
	localScale = gltfNode.scale.size() == 0 ? glm::mat4(1.0f)
		: glm::scale(glm::mat4(1.0f), glm::vec3(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]));

	localTranslation = gltfNode.translation.size() == 0 ? glm::mat4(1.0f)
		: glm::translate(glm::mat4(1.0f),
			glm::vec3(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]));
}
glm::mat4 Node::GetTransform(Space space) const{
	glm::mat4 localTransform = localRotation * localScale * localTranslation;
	return space == Space::Global ? parentWorldTransform * localTransform : localTransform;
}

std::string Node::GetName() const {
	return name;
}
NodeType Node::GetType() const {
	return type;
}
void Node::AddChild(Node* node) {
	children.push_back(node);
}

std::vector<Node> Node::GetChildren() const{
	std::vector<Node> nodes = std::vector<Node>(children.size());
	for (Node* node : children) nodes.push_back(*node);
	return nodes;
}

void Node::Translate(glm::vec3 translation, Space space) {
	if (space == Space::Global) 
		translation = glm::inverse(parentWorldTransform) * glm::vec4(translation, 1.0f);
	localTranslation = glm::translate(localTranslation, translation);

	UpdateChildrenTransforms();
}
void Node::Scale(glm::vec3 factor, Space space) {
	if (space == Space::Global)
		factor = glm::inverse(parentWorldTransform) * glm::vec4(factor, 1.0f);
	localScale = glm::scale(localScale, factor);

	UpdateChildrenTransforms();
}
void Node::Rotate(float angleInDegrees, glm::vec3 axis, Space space) {
	if (space == Space::Global)
		axis = glm::normalize(glm::inverse(parentWorldTransform) * glm::vec4(axis, 0.0f));
	axis = glm::inverse(localRotation) * glm::vec4(axis, 1.0f);
	localRotation = glm::rotate(localRotation, glm::radians(angleInDegrees), axis);

	UpdateChildrenTransforms();
}
void Node::UpdateChildrenTransforms() {
	for (Node* child : children) {
		child->parentWorldTransform = GetTransform(Space::Global);
		child->UpdateChildrenTransforms();
	}
}
std::string Node::ToString(unsigned int depth) const{
	std::string str = "-" + name;

	if (type == NodeType::Node) str += " (Node)";
	else if (type == NodeType::Mesh) str += " (Mesh)";
	else if (type == NodeType::Camera) str += " (Camera)";
	else str += " (Unknown Type. This should not happen. someting is wrong.)";

	std::string depthStr;
	depth++;
	for (unsigned int i = 0; i < depth; i++) depthStr += " ";

	for (const Node* child : children) str += "\n" + depthStr + child->ToString(depth);
	return str;
}