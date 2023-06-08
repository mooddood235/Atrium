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
	orderedTriangles = std::vector<BVHTriangle>(); orderedTriangles.reserve(triangles.size());

	root = BuildRecursive(0, triangles.size() - 1);

	triangles.swap(orderedTriangles);
	orderedTriangles.resize(0);

	flatRepresentation = new FlatBVHNode[numNodes];
	int offset = 0;
	Flatten(root, &offset);

	GenerateSSBOs();
}
BVHNode* BVH::BuildRecursive(unsigned int begin, unsigned int end) {
	numNodes++;

	AABB aabb;
	AABB centroidAABB;
	for (const BVHTriangle& triangle : triangles) {
		aabb = AABB::Union(aabb, triangle.aabb);
		centroidAABB = AABB::Union(centroidAABB, triangle.aabb.GetCentroid());
	}
	unsigned int dim = centroidAABB.GetMaxDimension();

	if (begin == end || aabb.GetSurfaceArea() == 0 || centroidAABB.max[dim] == centroidAABB.min[dim])
		return CreateLeaf(begin, end, aabb, dim);

	int split;
	if (splitMethod == SplitMethod::EqualCounts) split = SplitEqualCounts(begin, end, aabb, dim);
	else split = SplitSAH(begin, end, aabb, centroidAABB);

	if (split == -1) return CreateLeaf(begin, end, aabb, dim);

	BVHNode* left = BuildRecursive(begin, split);
	BVHNode* right = BuildRecursive(split + 1, end);

	return new BVHNode(aabb, left, right, dim, 0, 0);
}
BVHNode* BVH::CreateLeaf(unsigned int begin, unsigned int end, const AABB& aabb, unsigned int dim) {
	orderedTriangles.insert(orderedTriangles.end(), triangles.begin() + begin, triangles.begin() + end + 1);
	return new BVHNode(aabb, nullptr, nullptr, dim, orderedTriangles.size() - (end - begin + 1), end - begin + 1);
}
void BVH::GenerateSSBOs() {
	#pragma pack(push, 1)
	struct GPUTriangle {
		unsigned int i0, i1, i2;
		float pad0;
		GPUTriangle() {}
		GPUTriangle(const BVHTriangle& bvhTriangle) {
			Triangle triangle = bvhTriangle.triangle;
			i0 = triangle.index0; i1 = triangle.index1; i2 = triangle.index2;
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
	#pragma pack(pop)

	GPUVertex* GPUVertices = new GPUVertex[vertices.size()];
	for (unsigned int i = 0; i < vertices.size(); i++) GPUVertices[i] = GPUVertex(vertices[i]);

	GPUTriangle* GPUTriangles = new GPUTriangle[triangles.size()];
	for (unsigned int i = 0; i < triangles.size(); i++) GPUTriangles[i] = GPUTriangle(triangles[i]);

	GPUBVHNode* GPUBVHNodes = new GPUBVHNode[numNodes];
	for (unsigned int i = 0; i < numNodes; i++) GPUBVHNodes[i] = GPUBVHNode(flatRepresentation[i]);

	glGenBuffers(1, &verticesSSBO);
	glGenBuffers(1, &trianglesSSBO);
	glGenBuffers(1, &nodesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, verticesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUVertex) * vertices.size(), GPUVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUTriangle) * triangles.size(), GPUTriangles, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, nodesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPUBVHNode) * numNodes, GPUBVHNodes, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] GPUVertices;
	delete[] GPUTriangles;
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
	triangles = std::vector<BVHTriangle>(); triangles.reserve(noAABBTriangles.size());

	for (const Triangle& triangle : noAABBTriangles) {
		Vertex v0 = vertices[triangle.index0];
		Vertex v1 = vertices[triangle.index1];
		Vertex v2 = vertices[triangle.index2];

		glm::vec3 min = glm::min(v0.position, glm::min(v1.position, v2.position));
		glm::vec3 max = glm::max(v0.position, glm::max(v1.position, v2.position));

		AABB aabb = AABB(min, max);

		triangles.push_back(BVHTriangle(triangle, aabb));
	}
}
int BVH::SplitEqualCounts(unsigned int begin, unsigned int end, const AABB& aabb, unsigned int dim){	
	unsigned int mid = begin + (end - begin) / 2;

	std::nth_element(triangles.begin() + begin, triangles.begin() + mid, triangles.begin() + end + 1,
		[dim](const BVHTriangle& t0, const BVHTriangle& t1) {
			return t0.aabb.GetCentroid()[dim] < t1.aabb.GetCentroid()[dim];
		}
		);
	return mid;
}

int BVH::SplitSAH(unsigned int begin, unsigned int end, const AABB& aabb, const AABB& centroidAABB){
	unsigned int dim = aabb.GetMaxDimension();

	const unsigned int numBuckets = 12;
	BVHSplitBucket buckets[numBuckets];

	for (unsigned int i = begin; i <= end; i++) {
		int bucket = numBuckets * centroidAABB.GetOffset(triangles[i].aabb.GetCentroid())[dim];
		if (bucket == numBuckets) bucket--;
		buckets[bucket].count++;
		buckets[bucket].aabb = AABB::Union(buckets[bucket].aabb, triangles[i].aabb);
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
	float leafCost = end - begin + 1;
	minCost = 1.0f / 2.0f + minCost / aabb.GetSurfaceArea();

	if (minCost < leafCost) {
		auto midIter = std::partition(triangles.begin() + begin, triangles.begin() + end + 1,
			[=](const BVHTriangle& bvhTriangle) {
				int bucket = numBuckets * centroidAABB.GetOffset(bvhTriangle.aabb.GetCentroid())[dim];
				if (bucket == numBuckets) bucket--;
				return bucket <= minCostSplitBucket;
			}
		);
		return midIter - triangles.begin() - 1;
	}
	return -1;
}


