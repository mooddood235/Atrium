#include "EnvironmentMap.h"
#include <stb/stb_image.h>
#include <iostream>

using namespace Atrium;

EnvironmentMap::EnvironmentMap(const std::string& path){
	LoadEnvironmentMap(path);
}
void EnvironmentMap::LoadEnvironmentMap(const std::string& path) {
	stbi_set_flip_vertically_on_load(true);

	glGenTextures(1, &mapID);
	glBindTexture(GL_TEXTURE_2D, mapID);

	int width, height, numChannels;
	float* data = stbi_loadf(path.c_str(), &width, &height, &numChannels, 0);

	if (!data) {
		std::cout << "Texture at path '" << path << "' failed to load." << std::endl;
		stbi_image_free(data);
		exit(-1);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
	stbi_image_free(data);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}
unsigned int EnvironmentMap::GetMapID() const {
	return mapID;
}