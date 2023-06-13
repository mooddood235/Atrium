const float PI = 3.14159265359;
const float EPSILON = 0.0000001;

float sdot(vec3 v0, vec3 v1){
	return max(0.0, dot(v0, v1));
}
mat3 GetTangentSpace(vec3 n){
    // Choose a helper vector for the cross product
    vec3 helper = vec3(1, 0, 0);
    if (abs(n.x) > 0.99) helper = vec3(0, 0, 1);
    // Generate vectors
    vec3 tangent = normalize(cross(n, helper));
    vec3 binormal = normalize(cross(n, tangent));
    return mat3(tangent, binormal, n);
}