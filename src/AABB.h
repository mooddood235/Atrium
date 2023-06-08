#pragma once
#include <glm/glm.hpp>

namespace Atrium {
	struct AABB {
	public:
		AABB();
		AABB(glm::vec3 min, glm::vec3 max);
		glm::vec3 GetCentroid() const;
		unsigned int GetMaxDimension() const;
		float GetSurfaceArea() const;
		glm::vec3 GetOffset(glm::vec3 v) const;

		static AABB Union(AABB aabb0, AABB aabb1);
		static AABB Union(AABB aabb, glm::vec3 v);

	public:
		glm::vec3 min;
		glm::vec3 max;
	};
}


