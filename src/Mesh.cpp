#include "Mesh.h"

using namespace Atrium;

Mesh::Mesh(const std::string& name) : Node(name){
	type = NodeType::Mesh;
	vertices = std::vector<Vertex>();
}