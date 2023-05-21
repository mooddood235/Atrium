#include "Node.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace Atrium;

Node::Node(const tinygltf::Node& gltfNode, const tinygltf::Node* gltfParentNode) {
	name = gltfNode.name;
	type = NodeType::Node;
	children = std::vector<Node*>();
	LoadTransformations(&gltfNode, lQuaternion, lScale, lTranslation);
	LoadTransformations(gltfParentNode, gQuaternion, gScale, gTranslation);
}
void Node::LoadTransformations(const tinygltf::Node* gltfNode,
	glm::fquat& quaternion, glm::vec3& scale, glm::vec3& translation) {

	quaternion = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f);
	translation = glm::vec3(0.0f);

	if (gltfNode && gltfNode->rotation.size() != 0)
		quaternion = glm::fquat(gltfNode->rotation[3], gltfNode->rotation[0], gltfNode->rotation[1], gltfNode->rotation[2]);
	if (gltfNode && gltfNode->scale.size() != 0)
		scale = glm::vec3(gltfNode->scale[0], gltfNode->scale[1], gltfNode->scale[2]);
	if (gltfNode && gltfNode->translation.size() != 0)
		translation = glm::vec3(gltfNode->translation[0], gltfNode->translation[1], gltfNode->translation[2]);
}
glm::mat4 Node::GetTransform(Space space) const{
	glm::fquat quaternion = space == Space::Global ? gQuaternion : lQuaternion;
	glm::vec3 scale = space == Space::Global ? gScale : lScale;
	glm::vec3 translation = space == Space::Global ? gTranslation : lTranslation;

	return
		glm::translate(glm::mat4(1.0f), translation)
		* glm::scale(glm::mat4(1.0f), scale)
		* glm::toMat4(quaternion);
}
glm::mat4 Node::GetParentWorldTransform() const{
	return GetTransform(Space::Global) * glm::inverse(GetTransform(Space::Local));
}
glm::mat3 Node::GetRotationMatrix(Space space) const{
	glm::fquat quaternion = space == Space::Global ? gQuaternion : lQuaternion;
	return glm::toMat3(quaternion);
}
void Node::Translate(glm::vec3 translation, Space space) {
	if (space == Space::Global) {
		gTranslation += translation;
		lTranslation += glm::vec3(glm::inverse(GetParentWorldTransform()) * glm::vec4(translation, 1.0f));
	}
	else {
		lTranslation += translation;
		gTranslation += glm::vec3(GetParentWorldTransform() * glm::vec4(translation, 1.0f));
	}
}
void Node::Scale(glm::vec3 factor, Space space) {
	if (space == Space::Global) {
		gScale *= factor;
		lScale *= glm::vec3(glm::inverse(GetParentWorldTransform()) * glm::vec4(factor, 1.0f));
	}
	else {
		lScale *= factor;
		gScale *= glm::vec3(GetParentWorldTransform() * glm::vec4(factor, 1.0f));
	}
}
void Node::Rotate(float angleInDegrees, glm::vec3 axis, Space space) {
	if (space == Space::Global) {
		gQuaternion = glm::rotate(gQuaternion, glm::radians(angleInDegrees), 
			glm::inverse(GetRotationMatrix(Space::Global)) * axis);

		// Take to parent space. Remove translation and scaling.
		axis = glm::normalize(glm::inverse(GetParentWorldTransform()) * glm::vec4(axis, 0.0f));
		lQuaternion = glm::rotate(lQuaternion, glm::radians(angleInDegrees), 
			glm::inverse(GetRotationMatrix(Space::Local)) * axis);
	}
	else {
		lQuaternion = glm::rotate(lQuaternion, glm::radians(angleInDegrees),
			glm::inverse(GetRotationMatrix(Space::Local)) * axis);

		// Take axis to world space. Remove translation and scaling.
		axis = glm::normalize(GetParentWorldTransform() * glm::vec4(axis, 0.0f));
		gQuaternion = glm::rotate(gQuaternion, glm::radians(angleInDegrees),
			glm::inverse(GetRotationMatrix(Space::Global)) * axis);
	}
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