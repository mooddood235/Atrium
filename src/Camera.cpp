#include "Camera.h"
#include <iostream>
#include <glm/glm.hpp>

using namespace Atrium;

Camera::Camera(const tinygltf::Node& gltfNode, glm::mat4 parentWorldTransform, const tinygltf::Model& model) : Node(gltfNode, parentWorldTransform){
	if (gltfNode.camera < 0) {
		std::cout << "Tried to create Camera node from non-camera gltf node '" << gltfNode.name << "'!" << std::endl;
		exit(-1);
	}
	type = NodeType::Camera;
	LoadProjectionMatrix(gltfNode, model);
}
void Atrium::Camera::LoadProjectionMatrix(const tinygltf::Node& gltfNode, const tinygltf::Model& model){
	const tinygltf::Camera camera = model.cameras[gltfNode.camera];

	if (camera.type == "perspective") {
		projectionMatrix = glm::perspective(
			camera.perspective.yfov,
			camera.perspective.aspectRatio,
			camera.perspective.znear, camera.perspective.zfar);
	}
	else {
		projectionMatrix = glm::ortho(
			0.0, camera.orthographic.xmag,
			0.0, camera.orthographic.ymag,
			camera.orthographic.znear, camera.orthographic.zfar);
	}
}
glm::mat4 Camera::GetProjectionMatrix() const{
	return projectionMatrix;
}
