#include "Node.h"
#include <iostream>

using namespace Atrium;

Node::Node(const tinygltf::Node& gltfNode, Transform parentTransform) {
	name = gltfNode.name;
	type = NodeType::Node;
	children = std::vector<Node*>();

	this->parentTransform = parentTransform;
	LoadLocalTransform(gltfNode);
}
void Node::LoadLocalTransform(const tinygltf::Node& gltfNode) {
	localTransform = Transform();

	if (gltfNode.rotation.size() != 0)
		localTransform.rotationMatrix =
		glm::toMat3(glm::fquat(gltfNode.rotation[3], gltfNode.rotation[0], gltfNode.rotation[1], gltfNode.rotation[2]));

	if (gltfNode.scale.size() != 0)
		localTransform.scaleMatrix = glm::scale(glm::mat4(1.0f),
		glm::vec3(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]));

	if (gltfNode.translation.size() != 0)
		localTransform.translationMatrix = glm::translate(glm::mat4(1.0f),
		glm::vec3(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]));
}
Transform Node::GetTransform(Space space) const{
	if (space == Space::Local) return localTransform;
	else return parentTransform * localTransform;
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

void Node::Translate(glm::vec3 translation, Space space) {
	if (space == Space::Global) 
		translation = glm::inverse(parentTransform.rotationMatrix) * translation;
	localTransform.translationMatrix = glm::translate(localTransform.translationMatrix, translation);

	UpdateChildrenTransforms();
}
void Node::Scale(glm::vec3 factor, Space space) {
	if (space == Space::Global)
		factor = glm::inverse(parentTransform.scaleMatrix * parentTransform.rotationMatrix) * factor;
	localTransform.scaleMatrix = glm::scale(glm::mat4(localTransform.scaleMatrix), factor);

	UpdateChildrenTransforms();
}
void Node::Rotate(float angleInDegrees, glm::vec3 axis, Space space) {
	if (space == Space::Global)
		axis = glm::inverse(parentTransform.rotationMatrix) * axis;
	axis = glm::inverse(localTransform.rotationMatrix) * axis;
	localTransform.rotationMatrix =
		glm::rotate(glm::mat4(localTransform.rotationMatrix), glm::radians(angleInDegrees), axis);

	UpdateChildrenTransforms();
}
void Node::UpdateChildrenTransforms() {
	for (Node* child : children) {
		child->parentTransform = GetTransform(Space::Global);
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