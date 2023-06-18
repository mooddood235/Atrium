#include "BVHTriangle.h"

using namespace Atrium;


BVHTriangle::BVHTriangle(){
	triangle = Triangle();
	aabb = AABB();
	materialIndex = -1;
}

BVHTriangle::BVHTriangle(const Triangle& triangle, const AABB& aabb, unsigned int materialIndex){
	this->triangle = triangle;
	this->aabb = aabb;
	this->materialIndex = materialIndex;
}
