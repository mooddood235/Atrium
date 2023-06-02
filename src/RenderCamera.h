#pragma once

#include <glm/glm.hpp>
#include "Camera.h"
#include "Film.h"
#include "AStructure.h"
#include "ShaderProgram.h"
#include "Texture.h"

namespace Atrium {
	class RenderCamera {
	public:
		static void Init();
		static void Render(const Film& film, const Camera& camera, const AStructure& aStructure, const Texture& environmentMap);
	private:
		static ShaderProgram integrator;
	};
}


