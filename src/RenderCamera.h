#pragma once

#include <glm/glm.hpp>
#include "Camera.h"
#include "Film.h"
#include "AccelerationStructure.h"
#include "ShaderProgram.h"

namespace Atrium {
	class RenderCamera{
	public:
		RenderCamera(
			const Camera& camera,
			unsigned int resolutionX,
			unsigned int resolutionY
		);
		void SetCamera(const Camera& camera);
		unsigned int GetFilm();
		void Render();
	private:
		glm::mat4 modelMatrix;
		glm::mat4 projectionMatrix;
		
		Film film;
		ShaderProgram integrator;
	};
}


