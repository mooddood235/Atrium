#include "BVHNode.h"

using namespace Atrium;

BVHNode::BVHNode(const AABB& aabb, BVHNode* left, BVHNode* right, int splitAxis, int offset, int count){
	this->aabb = aabb;
	this->children[0] = left;
	this->children[1] = right;
	this->splitAxis = splitAxis;
	this->offset = offset;
	this->count = count;

}
