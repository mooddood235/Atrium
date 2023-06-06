#pragma once

#include "AABB.h"

namespace Atrium {
	struct BVHNode {
	public:
		BVHNode(const AABB& aabb, BVHNode* left, BVHNode* right, int splitAxis, int offset, int count);
	public:
		AABB aabb;
		BVHNode* children[2];
		int splitAxis, offset, count;
	};
}



