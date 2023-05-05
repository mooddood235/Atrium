#pragma once
#include <string>
#include <TinyGLTF/tiny_gltf.h>

#include "Node.h"

namespace Atrium {
	class Scene{
	public:
		Scene(const std::string& scenePath);
	private:
		void LoadModel(tinygltf::Model& model, const std::string& scenePath);
		Node* CreateHierarchy(const tinygltf::Model& model);
		Node* CreateNode(const tinygltf::Node& gltfNode, const tinygltf::Model& model, const tinygltf::Scene gltfScene);
	private:
		Node* rootNode;
	};
}

