#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Atrium {
	enum class Space {
		Global,
		Local
	};

	struct Transform{
	public:
		Transform();
		Transform(glm::fquat rotation, glm::vec3 scale, glm::vec3 translation);

		glm::mat4 GetMatrix() const;
		Transform operator*(Transform obj) const;
	public:
		glm::mat3 rotationMatrix;
		glm::mat3 scaleMatrix;
		glm::mat4 translationMatrix;
	};
}


