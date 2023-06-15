#pragma once

#include <span>

#include "AStructure.h"
#include "Scene.h"
#include "Vertex.h"
#include "Triangle.h"
#include "BVHNode.h"
#include "FlatBVHNode.h"
#include "BVHTriangle.h"

namespace Atrium {
	enum class SplitMethod {
		EqualCounts,
		SAH
	};

	class BVH : public AStructure{
	public:
		BVH(const Scene& scene, SplitMethod splitMethod);
		void Bind() const override;
		unsigned int GetDepth() const;
	private:
		void LoadMeshes(const Scene& scene);
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
		std::vector<BVHTriangle> bvhTriangles;
		std::vector<BVHTriangle> orderedBVHTriangles;

		unsigned int nodesSSBO;
	};
}



