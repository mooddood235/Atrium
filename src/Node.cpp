#include "Node.h"

using namespace Atrium;

Node::Node(const std::string& name) {
	this->name = name;
	children = std::vector<Node*>();
}