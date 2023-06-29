#include "BVH.h"

#include <glad/glad.h>
#include <algorithm>
#include <iostream>

#include "AABB.h"
#include "BVHSplitBucket.h"


using namespace Atrium;

BVH::BVH() {}

BVH::BVH(const std::vector<Node*> sceneHierarchy, SplitMethod splitMethod) {
	this->splitMethod = splitMethod;
	numNodes = 0;

	LoadMeshes(sceneHierarchy);
	orderedBVHTriangles = std::vector<BVHTriangle>(); orderedBVHTriangles.reserve(bvhTriangles.size());

	root = BuildRecursive(std::span<BVHTriangle>(bvhTriangles));

	bvhTriangles.swap(orderedBVHTriangles);
	orderedBVHTriangles.resize(0);

	flatRepresentation = new FlatBVHNode[numNodes];
	int offset = 0;
	Flatten(root, &offset);

	GenerateSSBOs();
}
BVHNode* BVH::BuildRecursive(std::span<BVHTriangle> triangles) {
	numNodes++;

	AABB aabb;
	AABB centroidAABB;
	for (const BVHTriangle& triangle : triangles) {
		aabb = AABB::Union(aabb, triangle.aabb);
		centroidAABB = AABB::Union(centroidAABB, triangle.aabb.GetCentroid());
	}
	unsigned int dim = centroidAABB.GetMaxDimension();

	if (triangles.size() == 1 || aabb.GetSurfaceArea() == 0 || centroidAABB.max[dim] == centroidAABB.min[dim])
		return CreateLeaf(triangles, aabb, dim);

	int split;
	if (splitMethod == SplitMethod::EqualCounts) split = SplitEqualCounts(triangles, aabb, dim);
	else split = SplitSAH(triangles, aabb, centroidAABB);

	if (split == -1) return CreateLeaf(triangles, aabb, dim);

	BVHNode* left = BuildRecursive(triangles.subspan(0, split));
	BVHNode* right = BuildRecursive(triangles.subspan(split));

	return new BVHNode(aabb, left, right, dim, 0, 0);
}
BVHNode* BVH::CreateLeaf(std::span<BVHTriangle> triangles, const AABB& aabb, unsigned int dim) {
	orderedBVHTriangles.insert(orderedBVHTriangles.end(), triangles.begin(), triangles.end());
	return new BVHNode(aabb, nullptr, nullptr, dim, orderedBVHTriangles.size() - triangles.size(), triangles.size());
}
int BVH::SplitEqualCounts(std::span<BVHTriangle> triangles, const AABB& aabb, unsigned int dim) {
	unsigned int mid = triangles.size() / 2;

	std::nth_element(triangles.begin(), triangles.begin() + mid, triangles.end(),
		[dim](const BVHTriangle& t0, const BVHTriangle& t1) {
			return t0.aabb.GetCentroid()[dim] < t1.aabb.GetCentroid()[dim];
		}
	);
	return mid;
}

int BVH::SplitSAH(std::span<BVHTriangle> triangles, const AABB& aabb, const AABB& centroidAABB) {
	unsigned int dim = centroidAABB.GetMaxDimension();

	const unsigned int numBuckets = 24;
	BVHSplitBucket buckets[numBuckets];

	for (const BVHTriangle& triangle : triangles) {
		int bucket = numBuckets * centroidAABB.GetOffset(triangle.aabb.GetCentroid())[dim];
		if (bucket == numBuckets) bucket--;
		buckets[bucket].count++;
		buckets[bucket].aabb = AABB::Union(buckets[bucket].aabb, triangle.aabb);
	}
	const unsigned int numSplits = numBuckets - 1;
	float costs[numSplits] = { 0 };

	unsigned int countBelow = 0;
	AABB aabbBelow;
	for (unsigned int i = 0; i < numSplits; i++) {
		aabbBelow = AABB::Union(aabbBelow, buckets[i].aabb);
		countBelow += buckets[i].count;
		costs[i] += countBelow * aabbBelow.GetSurfaceArea();
	}
	unsigned int countAbove = 0;
	AABB aabbAbove;
	for (unsigned int i = numSplits; i >= 1; i--) {
		aabbAbove = AABB::Union(aabbAbove, buckets[i].aabb);
		countAbove += buckets[i].count;
		costs[i - 1] += countAbove * aabbAbove.GetSurfaceArea();
	}
	unsigned int minCostSplitBucket = 0;
	float minCost = INFINITY;
	for (unsigned int i = 0; i < numSplits; i++) {
		if (costs[i] < minCost) {
			minCostSplitBucket = i;
			minCost = costs[i];
		}
	}
	float leafCost = triangles.size();
	minCost = 1.0f / 2.0f + minCost / aabb.GetSurfaceArea();

	if (minCost < leafCost) {
		auto midIter = std::partition(triangles.begin(), triangles.end(),
			[=](const BVHTriangle& bvhTriangle) {
				int bucket = numBuckets * centroidAABB.GetOffset(bvhTriangle.aabb.GetCentroid())[dim];
				if (bucket == numBuckets) bucket--;
				return bucket <= minCostSplitBucket;
			}
		);
		return midIter - triangles.begin();
	}
	return -1;
}

void BVH::GenerateSSBOs() {
	#pragma pack(push, 1)
	struct GPUVertex {
		glm::vec3 p;
		float pad0;
		glm::vec3 n;
		float pad1;
		GPUVertex() {}
		GPUVertex(const Vertex& vertex) {
			p = vertex.position;
			n = vertex.normal;
		}
	};
	struct GPUTriangle {
		unsigned int i0, i1, i2;
		unsigned int matIndex;
		GPUTriangle() {}
		GPUTriangle(const BVHTriangle& bvhTriangle) {
			i0 = bvhTriangle.triangle.index0; i1 = bvhTriangle.triangle.index1; i2 = bvhTriangle.triangle.index2;
			matIndex = bvhTriangle.materialIndex;
		}
	};
	struct GPUBVHNode {
		glm::vec3 min;
		float pad0;
		glm::vec3 max;
		float pad1;
		unsigned int trianglesOffset;
		unsigned int secondChildOffset;
		unsigned int trianglesCount;
		unsigned int splitAxis;

		GPUBVHNode() {}
		GPUBVHNode(const FlatBVHNode& flatBVHNode) {
			min = flatBVHNode.aabb.min;
			max = flatBVHNode.aabb.max;
			trianglesOffset = flatBVHNode.trianglesOffset;
			secondChildOffset = flatBVHNode.secondChildOffset;
			trianglesCount = flatBVHNode.trianglesCount;
			splitAxis = flatBVHNode.splitAxis;
		}
	};
	struct GPUMaterial {
		glm::vec3 albedo;
		float pad0;
		glm::vec3 emission;
		float pad1;
		float roughness;
		float metallic;
		float transmission;
		float ior;

		GPUMaterial() {}
		GPUMaterial(const Material& material) {
			albedo = material.albedoFactor;
			emission = material.emissiveFactor;
			roughness = material.roughnessFactor;
			metallic = material.metallicFactor;
			transmission = material.transmissionFactor;
			ior = material.ior;
		}
	};
	#pragma pack(pop)

	GPUVertex* GPUVertices = new GPUVertex[vertices.size()];
	for (unsigned int i = 0; i < vertices.size(); i++) GPUVertices[i] = GPUVertex(vertices[i]);

	GPUTriangle* GPUTriangles = new GPUTriangle[bvhTriangles.size()];
	for (unsigned int i = 0; i < bvhTriangles.size(); i++) GPUTriangles[i] = GPUTriangle(bvhTriangles[i]);

	GPUBVHNode* GPUBVHNodes = new GPUBVHNode[numNodes];
	for (unsigned int i = 0; i < numNodes; i++) GPUBVHNodes[i] = GPUBVHNode(flatRepresentation[i]);

	GPUMaterial* GPUMaterials = new GPUMaterial[materials.size()];
	for (unsigned int i = 0; i < materials.size(); i++) GPUMaterials[i] = GPUMaterial(materials[i]);

	glGenBuffers(1, &verticesSSBO);
	glGenBuffers(1, &trianglesSSBO);
	glGenBuffers(1, &nodesSSBO);
	glGenBuffers(1, &materialsSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, verticesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUVertex) * vertices.size(), GPUVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUTriangle) * bvhTriangles.size(), GPUTriangles, GL_STATIC_DRAW);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, nodesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUBVHNode) * numNodes, GPUBVHNodes, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUMaterial) * materials.size(), GPUMaterials, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] GPUVertices;
	delete[] GPUTriangles;
	delete[] GPUBVHNodes;
	delete[] GPUMaterials;
}
int BVH::Flatten(const BVHNode* bvhNode, int* offset){
	FlatBVHNode* flatBVHNode = flatRepresentation + *offset;

	int nodeOffset = (*offset)++;

	flatBVHNode->aabb = bvhNode->aabb;
	flatBVHNode->trianglesOffset = bvhNode->offset;
	flatBVHNode->trianglesCount = bvhNode->count;
	flatBVHNode->splitAxis = bvhNode->splitAxis;

	if (bvhNode->count == 0) {
		Flatten(bvhNode->children[0], offset);
		flatBVHNode->secondChildOffset = Flatten(bvhNode->children[1], offset);
	}
	return nodeOffset;
}
void BVH::Bind() const{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, verticesSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, trianglesSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, nodesSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, materialsSSBO);
}
void BVH::LoadMeshes(const std::vector<Node*> sceneHierarchy) {
	for (const Node* node : sceneHierarchy)
		if (node->GetType() == NodeType::Mesh)
			LoadMeshesHelper((const Mesh*)node);
}
void BVH::LoadMeshesHelper(const Mesh* mesh) {
	unsigned int oldVerticesSize = vertices.size();
	
	glm::mat4 modelMatrix = mesh->GetTransform(Space::Global).GetMatrix();
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(modelMatrix));

	vertices.reserve(vertices.size() + mesh->vertices.size());

	for (Vertex vertex : mesh->vertices) {
		vertex.position = modelMatrix * glm::vec4(vertex.position, 1.0f);
		vertex.normal = glm::normalize(normalMatrix * vertex.normal);
		vertices.push_back(vertex);
	}
	bvhTriangles.reserve(bvhTriangles.size() + mesh->indices.size() / 3);
	materials.push_back(mesh->material);
	for (unsigned int i = 0; i < mesh->indices.size(); i += 3) {
		Triangle triangle = Triangle(mesh->indices[i], mesh->indices[i + 1], mesh->indices[i + 2]) + oldVerticesSize;

		glm::vec3 aabbMin = glm::min(vertices[triangle.index0].position,
			glm::min(vertices[triangle.index1].position, vertices[triangle.index2].position));
		glm::vec3 aabbMax = glm::max(vertices[triangle.index0].position,
			glm::max(vertices[triangle.index1].position, vertices[triangle.index2].position));

		bvhTriangles.push_back(BVHTriangle(triangle, AABB(aabbMin, aabbMax), materials.size() - 1));
	}
	for (const Node* node : mesh->children)
		if (node->GetType() == NodeType::Mesh)
			LoadMeshesHelper((const Mesh*)node);
}

unsigned int BVH::GetDepth() const {
	return GetDepthHelper(root);
}
unsigned int BVH::GetDepthHelper(const BVHNode* node) const {
	if (!node) return 0;
	return 1 + glm::max(GetDepthHelper(node->children[0]), GetDepthHelper(node->children[1]));
}