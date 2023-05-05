#include "Scene.h"
#include <iostream>
#include <TinyGLTF/tiny_gltf.h>

using namespace Atrium;

Scene::Scene(const std::string& scenePath) {
	tinygltf::Model model;
	LoadModel(model, scenePath);
	rootNode = CreateHierarchy(model);
}
void Scene::LoadModel(tinygltf::Model& model, const std::string& scenePath) {
	tinygltf::TinyGLTF loader;
	std::string error;
	std::string warning;

	bool ret = loader.LoadASCIIFromFile(&model, &error, &warning, scenePath);

	if (!ret || !error.empty()) {
		std::cout << "LOAD GLTF ERROR:" << std::endl;
		std::cout << error << std::endl;
		exit(-1);
	}
	if (!warning.empty()) std::cout << warning << std::endl;
}
Node* Scene::CreateHierarchy(const tinygltf::Model& model) {
	tinygltf::Scene defaultScene = model.defaultScene >= 0 ? model.scenes[model.defaultScene] : model.scenes[0];
	return CreateNode(model.nodes[defaultScene.nodes[0]], model, defaultScene);
}
Node* Scene::CreateNode(const tinygltf::Node& gltfNode, const tinygltf::Model& model, const tinygltf::Scene gltfScene) {
	Node* node = new Node(gltfNode.name);

	for (int child : gltfNode.children) {
		tinygltf::Node gltfChildNode = model.nodes[gltfScene.nodes[child]];
		node->children.push_back(CreateNode(gltfChildNode, model, gltfScene));
	}
	return node;
}