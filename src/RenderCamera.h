#pragma once

#include <glm/glm.hpp>
#include "Scene/Camera.h"
#include "Texture/Film.h"
#include "ShaderProgram.h"
#include "Texture/EnvironmentMap.h"
#include "Scene/Scene.h"

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
			const EnvironmentMap& environmentMap,
			unsigned int samples,
			unsigned int depth,
			unsigned int seed,
			RenderMode renderMode);
		static void BindScene(const Scene& scene);
		static void UnBindScene(const Scene& scene);
	private:
		static ShaderProgram integrator;
		static bool sceneIsBound;
	};
}


