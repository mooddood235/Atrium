#include "Node.h"
#include <iostream>

using namespace Atrium;

Node::Node(const std::string& name) {
	this->name = name;
	children = std::vector<Node*>();
}
std::string Node::ToString(unsigned int depth) {
	std::string str = "-" + name;

	std::string depthStr;
	depth++;
	for (unsigned int i = 0; i < depth; i++) depthStr += " ";

	for (Node* child : children) str += "\n" + depthStr + child->ToString(depth);
	return str;
}