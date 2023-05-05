#pragma once

#include <string>
#include <vector>

namespace Atrium {
	class Node {
	public:
		Node(const std::string& name);
	public:
		std::string name;
		std::vector<Node*> children;
	};
}


