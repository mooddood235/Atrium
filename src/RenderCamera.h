#pragma once

#include <glm/glm.hpp>
#include "Camera.h"

namespace Atrium {
	class RenderCamera{
	public:
		RenderCamera(const Camera& camera);
		void SetCamera(const Camera& camera);
		
	private:
		glm::mat4 modelMatrix;
		glm::mat4 projectionMatrix;
	};
}


