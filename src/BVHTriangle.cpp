#include "BVHTriangle.h"

using namespace Atrium;


BVHTriangle::BVHTriangle(){
	triangle = Triangle();
	aabb = AABB();
}

BVHTriangle::BVHTriangle(const Triangle& triangle, const AABB& aabb){
	this->triangle = triangle;
	this->aabb = aabb;
}
