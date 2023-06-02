#pragma once

#include "Node.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <TinyGLTF/tiny_gltf.h>


namespace Atrium {
	class Camera : public Node{
	public:
		Camera(const tinygltf::Node& gltfNode, Transform parentTransform, const tinygltf::Model& model);
		glm::mat4 GetProjectionMatrix() const;
		void TransformFromInput(GLFWwindow* window, float deltaTime);
	private:
		void LoadProjectionMatrix(const tinygltf::Node& gltfNode, const tinygltf::Model& model);
	private:
		glm::mat4 projectionMatrix;
		float lastX = 0.0f, lastY = 0.0f;
		const float moveSpeed = 15.0f, mouseSensitivity = 0.05f;
		bool firstMouse = false;
	};
};

