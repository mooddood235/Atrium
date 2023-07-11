#pragma once
#include <glm/glm.hpp>

namespace Atrium {
	struct Film {
	public:
		Film();
		Film(unsigned int resolutionX, unsigned int resolutionY);
		unsigned int GetTextureID() const;
		glm::uvec2 GetResolution() const;
	private:
		void CreateTexture(unsigned int resolutionX, unsigned int resolutionY);
	private:
		unsigned int textureID;
		glm::uvec2 resolution;
	};
}


