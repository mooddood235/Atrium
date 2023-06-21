#pragma once
#include "ShaderProgram.h"
#include <glm/glm.hpp>
#include <limits>

namespace Atrium {
	struct EnvironmentMap{
	public:
		EnvironmentMap(const std::string& path);
		unsigned int GetMapID() const;
	private:
		void LoadEnvironmentMap(const std::string& path);
	private:
		unsigned int mapID;
		std::vector<glm::vec4> bins;
	};
}

