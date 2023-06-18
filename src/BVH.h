#pragma once

#include <span>

#include "Vertex.h"
#include "BVHNode.h"
#include "FlatBVHNode.h"
#include "BVHTriangle.h"
#include "Mesh.h"
#include "Material.h"

namespace Atrium {
	enum class SplitMethod {
		EqualCounts,
		SAH
	};
	class BVH{
	public:
		BVH();
		BVH(const std::vector<Node*> sceneHierarchy, SplitMethod splitMethod);
		void Bind() const;
		unsigned int GetDepth() const;
	private:
		void LoadMeshes(const std::vector<Node*> sceneHierarchy);
		void LoadMeshesHelper(const Mesh* mesh);
		BVHNode* BuildRecursive(std::span<BVHTriangle> triangles);
		BVHNode* CreateLeaf(std::span<BVHTriangle> triangles, const AABB& aabb, unsigned int dim);
		int Flatten(const BVHNode* bvhNode, int* offset);

		int SplitEqualCounts(std::span<BVHTriangle> triangles, const AABB& aabb, unsigned int dim);
		int SplitSAH(std::span<BVHTriangle> triangles, const AABB& aabb, const AABB& centroidAABB);

		void GenerateSSBOs();

		unsigned int GetDepthHelper(const BVHNode* node) const;
	private:
		BVHNode* root;
		FlatBVHNode* flatRepresentation;

		unsigned int numNodes;

		SplitMethod splitMethod;
		std::vector<Vertex> vertices;
		std::vector<BVHTriangle> bvhTriangles;
		std::vector<BVHTriangle> orderedBVHTriangles;
		std::vector<Material> materials;

		unsigned int verticesSSBO, trianglesSSBO, nodesSSBO, materialsSSBO;
	};
}



