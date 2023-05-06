#include "Mesh.h"

using namespace Atrium;

Mesh::Mesh(const std::string& name) : Node(name) {
	vertices = std::vector<Vertex>();
}