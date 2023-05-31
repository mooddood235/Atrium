#pragma once

#include <glm/glm.hpp>
#include "Camera.h"
#include "Film.h"
#include "AccelerationStructure.h"
#include "ShaderProgram.h"
#include "Texture.h"

namespace Atrium {
	class RenderCamera {
	public:
		static void Init();
		static void Render(const Camera& camera, const Film& film, const Texture& environmentMap);
	private:
		static ShaderProgram integrator;
	};
}


