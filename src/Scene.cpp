#include "Scene.h"
#include <iostream>
#include <TinyGLTF/tiny_gltf.h>

#include "Mesh.h"
#include "Camera.h"

using namespace Atrium;

Scene::Scene(const std::string& scenePath) {
	tinygltf::Model model;
	LoadModel(model, scenePath);
	CreateHierarchy(model);
	bvh = BVH(hierarchy, SplitMethod::SAH);
}
std::string Scene::ToString() const{
	std::string str;
	for (unsigned int i = 0; i < hierarchy.size(); i++) {
		if (i > 0) str += "\n";
		str += hierarchy[i]->ToString();
	}
	return str;
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
void Scene::CreateHierarchy(const tinygltf::Model& model) {
	const tinygltf::Scene defaultScene = model.defaultScene >= 0 ? model.scenes[model.defaultScene] : model.scenes[0];
	for (int node : defaultScene.nodes) hierarchy.push_back(CreateNode(model.nodes[node], Transform(), model));
}
Node* Scene::CreateNode(const tinygltf::Node& gltfNode, Transform parentTransform, const tinygltf::Model& model) {
	Node* node;
	if (gltfNode.mesh >= 0) node = new Mesh(gltfNode, parentTransform, model);
	else if (gltfNode.camera >= 0) {
		node = new Camera(gltfNode, parentTransform, model);
		cameras.push_back((Camera*)node);
	}
	else node = new Node(gltfNode, parentTransform);

	for (int child : gltfNode.children) {
		const tinygltf::Node gltfChildNode = model.nodes[child];
		node->AddChild(CreateNode(gltfChildNode, node->GetTransform(Space::Global), model));
	}
	return node;
}