#pragma once
#include <string>
#include <TinyGLTF/tiny_gltf.h>
#include <ostream>

#include "Node.h"

namespace Atrium {
	class Scene{
	public:
		Scene(const std::string& scenePath);
		std::string ToString() const;
	private:
		void LoadModel(tinygltf::Model& model, const std::string& scenePath);
		void CreateHierarchy(const tinygltf::Model& model);
		Node* CreateNode(const tinygltf::Node& gltfNode, const tinygltf::Node* gltfParentNode, const tinygltf::Model& model) const;
	private:
		std::vector<Node*> hierarchy;
	};
}

