#pragma once
#include "ShaderProgram.h"
#include <glm/glm.hpp>

namespace Atrium {
	struct EnvironmentMap{
	public:
		EnvironmentMap(const std::string& path, const std::string& debugPath = "");
		void SaveDebug(glm::vec3* data, const std::string& debugPath);
		unsigned int GetMapID() const;
		void BindBins() const;
		glm::uvec2 GetDims() const;
	private:
		float* LoadEnvironmentMap(const std::string& path);
		void CreateBins(glm::vec3* data, unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1);
		void GenerateBinsSSBO();
	private:
		unsigned int mapID;
		unsigned int binsSSBO;
		std::vector<glm::uvec4> bins;
		
		int width, height;

		const float minRadiance = 10000.0f;
		const unsigned int smallestBinArea = 10;
	};
}

