#pragma once

#include "../Data/AABB.h"

namespace Atrium {
	struct BVHSplitBucket{
	public:
		BVHSplitBucket();
	public:
		unsigned int count;
		AABB aabb;
	};
}


