#include "BVH.h"
#include "AABB.h"
#include <algorithm>
#include <iostream>

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

	if (begin == end || aabb.GetSurfaceArea() == 0 || centroidAABB.max[dim] == centroidAABB.min[dim]) {
		orderedTriangles.insert(orderedTriangles.end(), triangles.begin() + begin, triangles.begin() + end + 1);
		return new BVHNode(aabb, nullptr, nullptr, dim, orderedTriangles.size() - (end - begin + 1), end - begin + 1);
	}

	unsigned int split = SplitEqualCounts(begin, end, aabb);

	BVHNode* left = BuildRecursive(begin, split);
	BVHNode* right = BuildRecursive(split + 1, end);

	return new BVHNode(aabb, left, right, dim, 0, 0);
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


unsigned int BVH::SplitEqualCounts(unsigned int begin, unsigned int end, const AABB& aabb){	
	unsigned int dim = aabb.GetMaxDimension();
	unsigned int mid = begin + (end - begin) / 2;

	std::nth_element(triangles.begin() + begin, triangles.begin() + mid, triangles.begin() + end,
		[dim](const BVHTriangle& t0, const BVHTriangle& t1) {
			return t0.aabb.GetCentroid()[dim] < t1.aabb.GetCentroid()[dim];
		}
		);
	return mid;
}


