#pragma once

#include <vector>
#include "Scene.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Mesh.h"

namespace Atrium {
	struct AStructure{
	public:
		virtual void Bind() const = 0;
	protected:
		void LoadMeshes(const Scene& scene, std::vector<Triangle>& triangles);
		void GenerateSSBOs(const std::vector<Triangle>& triangles);
	private:
		void LoadMeshesHelper(const Mesh* mesh, std::vector<Triangle>& triangles);
	protected:
		std::vector<Vertex> vertices;
		unsigned int verticesSSBO, trianglesSSBO;
	};
}



