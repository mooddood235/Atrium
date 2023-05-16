#pragma once
#include <string>
#include <vector>
#include <TinyGLTF/tiny_gltf.h>

#include "Vertex.h"
#include "Node.h"

namespace Atrium {
	class Mesh : public Node{
	public:
		Mesh(const std::string& name);
		Mesh(const std::string& name, const tinygltf::Mesh& mesh, const tinygltf::Model& model);
	public:
		std::vector<Vertex> vertices;
	};
}


