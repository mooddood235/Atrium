#pragma once
#include "AStructure.h"
#include <vector>
#include "Scene.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Mesh.h"
#include "ShaderProgram.h"

namespace Atrium {
	struct Buffer : public AStructure {
	public:
		Buffer();
		Buffer(const Scene& scene);
		void Bind() const override;
	private:
		std::vector<Triangle> triangles;
	};
}


