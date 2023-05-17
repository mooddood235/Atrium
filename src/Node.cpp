#include "Node.h"
#include <iostream>

using namespace Atrium;

Node::Node(const tinygltf::Node& gltfNode) {
	name = gltfNode.name;
	type = NodeType::Node;

	children = std::vector<Node*>();

	rotation = gltfNode.rotation.size() != 0 
		? glm::vec4(gltfNode.rotation[0], gltfNode.rotation[1], gltfNode.rotation[2], gltfNode.rotation[3])
		: glm::vec4(0.0f);
	scale = gltfNode.scale.size() != 0 
		? glm::vec3(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]) 
		: glm::vec3(1.0f);
	translation = gltfNode.translation.size() != 0 
		? glm::vec3(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]) 
		: glm::vec3(0.0f);
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