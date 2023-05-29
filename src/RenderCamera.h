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
		RenderCamera(
			const Camera& camera,
			unsigned int resolutionX,
			unsigned int resolutionY
		);
		void SetCamera(const Camera& camera);
		void SetEnvironmentMap(const Texture& environmentMap);
		unsigned int GetFilm() const;
		void Render() const;
	private:
		glm::mat4 modelMatrix;
		glm::mat4 projectionMatrix;
		
		Film film;
		ShaderProgram integrator;

		Texture environmentMap;
	};
}


