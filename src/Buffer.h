#pragma once
#include "AccelerationStructure.h"
#include <vector>
#include "Scene.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Mesh.h"

namespace Atrium {
	struct Buffer : public AccelerationStructure {
	public:
		Buffer();
		Buffer(const Scene& scene);
	private:
		void LoadMeshes(const Mesh* mesh);
	private:
		std::vector<Vertex> vertices;
		std::vector<Triangle> triangles;
	};
}


