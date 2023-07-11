#include "Transform.h"
#include <glm/gtx/matrix_decompose.hpp>

using namespace Atrium;


Transform::Transform() {
	rotationMatrix = glm::mat4(1.0f);
	scaleMatrix = glm::mat4(1.0f);
	translationMatrix = glm::mat4(1.0f);
}
Transform::Transform(glm::fquat rotation, glm::vec3 scale, glm::vec3 translation) {
	rotationMatrix = glm::toMat3(rotation);
	scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
	translationMatrix = glm::translate(glm::mat4(1.0f), translation);
}
glm::mat4 Transform::GetMatrix() const{
	return translationMatrix * glm::mat4(scaleMatrix * rotationMatrix);
}

Transform Transform::operator*(Transform obj) const{
	glm::mat4 newMatrix = GetMatrix() * obj.GetMatrix();

	glm::vec3 scale;
	glm::fquat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(newMatrix, scale, rotation, translation, skew, perspective);

	return Transform(rotation, scale, translation);
}
