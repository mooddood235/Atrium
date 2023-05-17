#include "Node.h"
#include <iostream>

using namespace Atrium;

Node::Node(const tinygltf::Node& gltfNode) {
	name = gltfNode.name;
	type = NodeType::Node;
	children = std::vector<Node*>();
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