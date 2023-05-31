#pragma once

#include <glm/glm.hpp>
#include "Camera.h"
#include "Film.h"
#include "AccelerationStructure.h"
#include "ShaderProgram.h"
#include "Texture.h"

namespace Atrium {
	class RenderCamera{
	public:
		RenderCamera(const Camera& camera);
		void SetCamera(const Camera& camera);
		void SetEnvironmentMap(const Texture& environmentMap);
		void Render(const Film& film) const;
	private:
		glm::mat4 modelMatrix;
		glm::mat4 projectionMatrix;
		
		ShaderProgram integrator;

		Texture environmentMap;
	};
}


