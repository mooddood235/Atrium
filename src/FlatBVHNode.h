#pragma once

#include "AABB.h"

namespace Atrium {
	struct FlatBVHNode{
		AABB aabb;
		int trianglesOffset;
		int secondChildOffset;
		unsigned int trianglesCount;
		unsigned int splitAxis;
	};
}



