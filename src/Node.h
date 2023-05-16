#pragma once

#include <string>
#include <vector>

namespace Atrium {
	enum class NodeType {
		Node,
		Mesh
	};

	class Node {
	public:
		Node(const std::string& name);
		std::string ToString(unsigned int depth = 0) const;
	public:
		std::string name;
		NodeType type;
		std::vector<Node*> children;
	};
}


