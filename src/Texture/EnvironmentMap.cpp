#include "EnvironmentMap.h"
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <iostream>

using namespace Atrium;

EnvironmentMap::EnvironmentMap(const std::string& path, const std::string& debugPath) {
	float* data = LoadEnvironmentMap(path);
	CreateBins((glm::vec3*)data, 0, width - 1, 0, height - 1);
	if (!debugPath.empty()) SaveDebug((glm::vec3*)data, debugPath);
	stbi_image_free(data);
	GenerateBinsSSBO();
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
void EnvironmentMap::CreateBins(glm::vec3* data, unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1) {
	unsigned int w = x1 - x0 + 1;
	unsigned int h = y1 - y0 + 1;

	glm::vec3 radiance = glm::vec3(0.0f);

	for (unsigned int x = x0; x <= x1; x++) {
		for (unsigned int y = y0; y <= y1; y++) {
			unsigned int flat = x + y * width;
			radiance += data[flat];
		}
	}
	if (glm::length(radiance) <= minRadiance || w * h <= smallestBinArea || w * h == 1) {
		bins.push_back(glm::uvec4(x0, x1, y0, y1));
		return;
	}
	if (w > h) {
		unsigned int xSplit = (w - 1) / 2 + x0;
		CreateBins(data, x0, xSplit, y0, y1);
		CreateBins(data, xSplit + 1, x1, y0, y1);
	}
	else {
		unsigned int ySplit = (h - 1) / 2 + y0;
		CreateBins(data, x0, x1, y0, ySplit);
		CreateBins(data, x0, x1, ySplit + 1, y1);
	}
}
void EnvironmentMap::SaveDebug(glm::vec3* data, const std::string& debugPath) {
	stbi_flip_vertically_on_write(true);
	for (const glm::uvec4& bin : bins) {
		unsigned int x0 = bin.x, x1 = bin.y, y0 = bin.z, y1 = bin.w;
		for (unsigned int x = x0; x <= x1; x++) {
			unsigned int flatBot = x + y0 * width;
			unsigned int flatTop = x + y1 * width;
			data[flatBot] = glm::uvec3(1, 0, 0);
			data[flatTop] = glm::uvec3(1, 0, 0);
		}
		for (unsigned int y = y0; y <= y1; y++) {
			unsigned int flatLeft = x0 + y * width;
			unsigned int flatRight = x1 + y * width;
			data[flatLeft] = glm::uvec3(1, 0, 0);
			data[flatRight] = glm::uvec3(1, 0, 0);
		}
	}
	stbi_write_hdr(debugPath.c_str(), width, height, 3, (float*)data);
}
void EnvironmentMap::GenerateBinsSSBO() {
	#pragma pack(push, 1)
	struct Bin { unsigned int x0, x1, y0, y1; };
	#pragma pack(pop)
	Bin* binsArray = new Bin[bins.size()];
	for (unsigned int i = 0; i < bins.size(); i++) binsArray[i] = Bin{ bins[i].x, bins[i].y, bins[i].z, bins[i].w };

	glGenBuffers(1, &binsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Bin) * bins.size(), binsArray, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] binsArray;
}
void EnvironmentMap::BindBins() const{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, binsSSBO);
}
glm::uvec2 EnvironmentMap::GetDims() const{
	return glm::uvec2(width, height);
}
unsigned int EnvironmentMap::GetMapID() const {
	return mapID;
}