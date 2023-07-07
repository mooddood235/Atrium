#pragma once

#include <string>

namespace Atrium {
	struct Texture{
	public:
		Texture();
		Texture(const std::string& path);
		unsigned int GetTextureID() const;
		bool IsNull() const;
		uint64_t GetTextureHandle() const;
	private:
		void LoadTexture(const std::string& path);
	protected:
		unsigned int textureID;
		std::string path;
		bool isNull;
	};
}


