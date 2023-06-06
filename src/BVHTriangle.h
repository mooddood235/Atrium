#pragma once
#include "Triangle.h"
#include "AABB.h"

namespace Atrium {
	struct BVHTriangle{
	public:
		BVHTriangle();
		BVHTriangle(const Triangle& triangle, const AABB& aabb);

		Triangle triangle;
		AABB aabb;
	};
}


