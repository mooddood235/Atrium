#pragma once

#include <glm/glm.hpp>
#include "Camera.h"
#include "Film.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Scene.h"

namespace Atrium {
	enum class RenderMode {
		Write = 0,
		Append = 1
	};

	class RenderCamera {
	public:
		static void Init();
		static void Render(
			const Film& film,
			const Camera& camera,
			const Scene& scene,
			const Texture& environmentMap,
			unsigned int samples,
			unsigned int depth,
			unsigned int seed,
			RenderMode renderMode);
	private:
		static ShaderProgram integrator;
	};
}


