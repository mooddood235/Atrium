#pragma once
#include <vector>
#include <TinyGLTF/tiny_gltf.h>
#include "Vertex.h"
#include "Material.h"

namespace Atrium {
	struct Primitive{
	public:
		Primitive(const tinygltf::Primitive& primitive, const tinygltf::Model& model);
	private:
		void LoadAttributes(const tinygltf::Primitive& primitive, const tinygltf::Model& model);
		void LoadIndices(const tinygltf::Primitive& primitive, const tinygltf::Model& model);
		void LoadMaterial(const tinygltf::Primitive& primitive, const tinygltf::Model& model);
		Texture GetTexture(const tinygltf::TextureInfo& textureInfo, const tinygltf::Model& model, bool isSRGB);
		Texture GetNormalTexture(const tinygltf::NormalTextureInfo& textureInfo, const tinygltf::Model& model);
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		Material material;
	};
}

