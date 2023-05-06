#pragma once
#include <string>
#include <vector>

#include "Vertex.h"
#include "Node.h"

namespace Atrium {
	class Mesh : public Node{
	public:
		Mesh(const std::string& name);
	public:
		std::vector<Vertex> vertices;
	};
}


