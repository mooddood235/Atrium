#include "BVH.h"

#include <algorithm>
#include <iostream>

#include "AABB.h"
#include "BVHSplitBucket.h"


using namespace Atrium;

BVH::BVH(const Scene& scene, SplitMethod splitMethod) {
	this->splitMethod = splitMethod;
	numNodes = 0;

	LoadMeshes(scene);
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
	#pragma pack(pop)

	std::vector<Triangle> triangles = std::vector<Triangle>();
	triangles.reserve(bvhTriangles.size());
	for (const BVHTriangle& bvhTriangle : bvhTriangles) triangles.push_back(bvhTriangle.triangle);
	AStructure::GenerateSSBOs(triangles);

	GPUBVHNode* GPUBVHNodes = new GPUBVHNode[numNodes];
	for (unsigned int i = 0; i < numNodes; i++) GPUBVHNodes[i] = GPUBVHNode(flatRepresentation[i]);

	glGenBuffers(1, &nodesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, nodesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUBVHNode) * numNodes, GPUBVHNodes, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] GPUBVHNodes;
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
}
void BVH::LoadMeshes(const Scene& scene) {
	std::vector<Triangle> noAABBTriangles;
	AStructure::LoadMeshes(scene, vertices, noAABBTriangles);
	bvhTriangles = std::vector<BVHTriangle>(); bvhTriangles.reserve(noAABBTriangles.size());

	for (const Triangle& triangle : noAABBTriangles) {
		Vertex v0 = vertices[triangle.index0];
		Vertex v1 = vertices[triangle.index1];
		Vertex v2 = vertices[triangle.index2];

		glm::vec3 min = glm::min(v0.position, glm::min(v1.position, v2.position));
		glm::vec3 max = glm::max(v0.position, glm::max(v1.position, v2.position));

		AABB aabb = AABB(min, max);

		bvhTriangles.push_back(BVHTriangle(triangle, aabb));
	}
}
unsigned int BVH::GetDepth() const {
	return GetDepthHelper(root);
}
unsigned int BVH::GetDepthHelper(const BVHNode* node) const {
	if (!node) return 0;
	return 1 + glm::max(GetDepthHelper(node->children[0]), GetDepthHelper(node->children[1]));
}