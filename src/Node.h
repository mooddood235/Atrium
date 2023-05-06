#pragma once

#include <string>
#include <vector>

namespace Atrium {
	class Node {
	public:
		Node(const std::string& name);
		std::string ToString(unsigned int depth = 0);
	public:
		std::string name;
		std::vector<Node*> children;
	};
}


