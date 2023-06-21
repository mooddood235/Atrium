#pragma once
#include "ShaderProgram.h"
#include <glm/glm.hpp>

namespace Atrium {
	struct EnvironmentMap{
	public:
		EnvironmentMap(const std::string& path);
		unsigned int GetMapID() const;
	private:
		float* LoadEnvironmentMap(const std::string& path);
		void CreateBins(const glm::vec3* data, unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1);
	private:
		unsigned int mapID;
		int width, height;
		std::vector<glm::vec4> bins;
		const float minRadiance = 1.0f;
		const float smallestBinArea = 1000.0f;
	};
}

