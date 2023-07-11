#include "Film.h"
#include <iostream>
#include <glad/glad.h>

using namespace Atrium;
Film::Film() {
	resolution = glm::uvec2(0);
	CreateTexture(0, 0);
}
Film::Film(unsigned int resolutionX, unsigned int resolutionY) {
	resolution = glm::uvec2(resolutionX, resolutionY);
	CreateTexture(resolutionX, resolutionY);
}
unsigned int Film::GetTextureID() const{
	return textureID;
}
void Film::CreateTexture(unsigned int resolutionX, unsigned int resolutionY) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolutionX, resolutionY, 0, GL_RGBA, GL_FLOAT, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}
glm::uvec2 Film::GetResolution() const {
	return resolution;
}