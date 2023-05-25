#pragma once
#include "AccelerationStructure.h"
#include <vector>
#include "Scene.h"
#include "Vertex.h"
#include "Triangle.h"

namespace Atrium {
	class Buffer : public AccelerationStructure {
	public:
		Buffer();
		Buffer(const Scene& scene);
	private:
		std::vector<Vertex> vertices;
		std::vector<Triangle> triangles;
	};
}


