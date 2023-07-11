#pragma once
#include "../Data/Triangle.h"
#include "../Data/AABB.h"

namespace Atrium {
	struct BVHTriangle{
	public:
		BVHTriangle();
		BVHTriangle(const Triangle& triangle, const AABB& aabb, unsigned int materialIndex);

		Triangle triangle;
		AABB aabb;
		unsigned int materialIndex;
	};
}


