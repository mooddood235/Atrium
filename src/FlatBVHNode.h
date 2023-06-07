#pragma once

#include "AABB.h"

namespace Atrium {
	struct FlatBVHNode{
		AABB aabb;
		unsigned int trianglesOffset;
		unsigned int secondChildOffset;
		unsigned int trianglesCount;
		unsigned int splitAxis;
	};
}



