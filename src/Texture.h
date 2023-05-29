#pragma once

#include <string>

namespace Atrium {
	struct Texture{
	public:
		Texture();
		Texture(const std::string& path);
		unsigned int GetTextureID() const;
	private:
		void LoadTexture(const std::string& path);
	private:
		unsigned int textureID;
		std::string path;
	};
}


