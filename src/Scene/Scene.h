#pragma once
#include <string>
#include <TinyGLTF/tiny_gltf.h>
#include <ostream>

#include "Node.h"
#include "Camera.h"
#include "../Data/Transform.h"
#include "../BVH/BVH.h"

namespace Atrium {
	class Scene{
	public:
		Scene(const std::string& scenePath);
		std::string ToString() const;
	private:
		void LoadModel(tinygltf::Model& model, const std::string& scenePath);
		void CreateHierarchy(const tinygltf::Model& model);
		Node* CreateNode(const tinygltf::Node& gltfNode, Transform parentTransform, const tinygltf::Model& model);
	public:
		std::vector<Node*> hierarchy;
		std::vector<Camera*> cameras;
		BVH bvh;
	};
}

