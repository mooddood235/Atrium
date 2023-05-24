#pragma once

#include "Node.h"
#include <TinyGLTF/tiny_gltf.h>

namespace Atrium {
	class Camera : public Node{
	public:
		Camera(const tinygltf::Node& gltfNode, glm::mat4 parentWorldTransform, const tinygltf::Model& model);
		glm::mat4 GetProjectionMatrix();
	private:
		void LoadProjectionMatrix(const tinygltf::Node& gltfNode, const tinygltf::Model& model);
	private:
		glm::mat4 projectionMatrix;
	};
};

