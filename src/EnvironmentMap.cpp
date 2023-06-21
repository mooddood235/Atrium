#include "EnvironmentMap.h"
#include <stb/stb_image.h>
#include <iostream>

using namespace Atrium;

EnvironmentMap::EnvironmentMap(const std::string& path){
	float* data = LoadEnvironmentMap(path);
	CreateBins((glm::vec3*)data, 0, width - 1, 0, height - 1);
	stbi_image_free(data);
}
float* EnvironmentMap::LoadEnvironmentMap(const std::string& path) {
	stbi_set_flip_vertically_on_load(true);

	glGenTextures(1, &mapID);
	glBindTexture(GL_TEXTURE_2D, mapID);

	int numChannels;
	float* data = stbi_loadf(path.c_str(), &width, &height, &numChannels, 0);

	if (!data) {
		std::cout << "Texture at path '" << path << "' failed to load." << std::endl;
		stbi_image_free(data);
		exit(-1);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	return data;
}
void EnvironmentMap::CreateBins(const glm::vec3* data, unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1) {
	unsigned int w = x1 - x0;
	unsigned int h = y1 - y0;

	glm::vec3 radiance = glm::vec3(0.0f);

	for (unsigned int x = x0; x <= x1; x++) {
		for (unsigned int y = y0; y <= y1; y++) {
			unsigned int flat = x + y * width;
			radiance += data[flat];
		}
	}
	if (glm::length(radiance) <= glm::length(minRadiance) || w * h <= smallestBinArea || w * h == 0) {
		bins.push_back(glm::vec4(x0, x1, y0, y1));
		return;
	}
	if (w > h) {
		unsigned int xSplit = w / 2 + x0;
		CreateBins(data, x0, xSplit, y0, y1);
		CreateBins(data, xSplit + 1, x1, y0, y1);
	}
	else {
		unsigned int ySplit = h / 2 + y0;
		CreateBins(data, x0, x1, y0, ySplit);
		CreateBins(data, x0, x1, ySplit + 1, y1);
	}
}
unsigned int EnvironmentMap::GetMapID() const {
	return mapID;
}