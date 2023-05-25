#include "Buffer.h"

using namespace Atrium;

Buffer::Buffer() {
	vertices = std::vector<Vertex>();
	triangles = std::vector<Triangle>();
}

Buffer::Buffer(const Scene& scene) {
	for (Node& node : scene.GetHierarchy()) {
		
	}
}
