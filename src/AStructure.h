#pragma once

#include <vector>
#include "Scene.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Mesh.h"

namespace Atrium {
	#pragma pack(push, 1)
	struct GPUVertex {
		glm::vec3 p;
		float pad0;
		glm::vec3 n;
		float pad1;
		GPUVertex();
		GPUVertex(const Vertex& vertex);
	};
	#pragma pack(pop)

	struct AStructure{
	public:
		virtual void Bind() const = 0;
	protected:
		void LoadMeshes(const Scene& scene, std::vector<Vertex>& vertices, std::vector<Triangle>& triangles);
	private:
		void LoadMeshesHelper(const Mesh* mesh, std::vector<Vertex>& vertices, std::vector<Triangle>& triangles);
	};
}



