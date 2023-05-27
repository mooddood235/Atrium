#include "Triangle.h"

using namespace Atrium;

Triangle::Triangle(unsigned int index0, unsigned int index1, unsigned int index2) {
	this->index0 = index0;
	this->index1 = index1;
	this->index2 = index2;
}

Triangle Triangle::operator+(unsigned int obj) const{
	return Triangle(index0 + obj, index1 + obj, index2 + obj);
}
