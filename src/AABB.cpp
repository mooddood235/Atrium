#include "AABB.h"
#include <limits>

using namespace Atrium;

AABB::AABB() {
    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::lowest());
}
AABB::AABB(glm::vec3 min, glm::vec3 max) {
    this->min = min;
    this->max = max;
}

AABB AABB::Union(AABB aabb0, AABB aabb1){
    return AABB(glm::min(aabb0.min, aabb1.min), glm::max(aabb0.max, aabb1.max));
}

AABB AABB::Union(AABB aabb, glm::vec3 v){
    return AABB(glm::min(aabb.min, v), glm::max(aabb.max, v));
}

glm::vec3 AABB::GetCentroid() const{
    return (min + max) / 2.0f;
}

unsigned int AABB::GetMaxDimension() const{
    float xDim = max.x - min.x;
    float yDim = max.y - min.y;
    float zDim = max.z - min.z;

    float maxDim = glm::max(xDim, glm::max(yDim, zDim));

    if (maxDim == xDim) return 0;
    else if (maxDim == yDim) return 1;
    return 2;
}

float AABB::GetSurfaceArea() const{
    glm::vec3 d = max - min;
    return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

glm::vec3 AABB::GetOffset(glm::vec3 v) const{
    glm::vec3 o = v - min;
    if (max.x > min.x)
        o.x /= max.x - min.x;
    if (max.y > min.y)
        o.y /= max.y - min.y;
    if (max.z > min.z)
        o.z /= max.z - min.z;
    return o;
}
