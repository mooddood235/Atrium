#include "Texture.h"
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <iostream>

using namespace Atrium;
Texture::Texture() {
	textureID = 0;
	path = "";
	isNull = true;
}
Texture::Texture(const std::string& path) {
	this->path = path;
	LoadTexture(path);
	isNull = false;
}

unsigned int Texture::GetTextureID() const{
	return textureID;
}
bool Texture::IsNull() const {
	return isNull;
}

void Texture::LoadTexture(const std::string& path) {
	stbi_set_flip_vertically_on_load(true);
	const char* pathCString = path.c_str();

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, numChannels;
	unsigned char* data = stbi_load(pathCString, &width, &height, &numChannels, 0);

	if (!data) {
		std::cout << "Texture at path '" << path << "' failed to load." << std::endl;
		stbi_image_free(data);
		exit(-1);
	}

	GLenum internalFormat = GL_RED, format = GL_RED;
	if (numChannels == 3) internalFormat = GL_RGB, format = GL_RGB;
	else if (numChannels == 4) internalFormat = GL_RGBA, format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}
