const float PI = 3.14159265359;
const float EPSILON = 0.0000001;

float sdot(vec3 v0, vec3 v1){
	return max(0.0, dot(v0, v1));
}
float AbsDot(vec3 v0, vec3 v1){
    return abs(dot(v0, v1));
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
float Sqr(float x) { return x * x; }
float SafeACos(float x) { return acos(clamp(x, -1.0, 1.0)); }
float SafeSqrt(float x) { return sqrt(max(0.0, x)); }
float CosTheta(vec3 w) { return w.z; }
float Cos2Theta(vec3 w) { return w.z * w.z; }
float AbsCosTheta(vec3 w) { return abs(w.z); }
float SCosTheta(vec3 w) { return max(0.0, w.z); }
float Sin2Theta(vec3 w) { return max(0.0, 1.0 - Cos2Theta(w)); }
float SinTheta(vec3 w) { return sqrt(Sin2Theta(w)); }
float TanTheta(vec3 w) { return SinTheta(w) / CosTheta(w); };
float Tan2Theta(vec3 w) { return Sin2Theta(w) / Cos2Theta(w); }
float CosPhi(vec3 w){
    float sinTheta = SinTheta(w);
    return sinTheta == 0.0 ? 1.0 : clamp(w.x / sinTheta, -1.0, 1.0);
}
float SinPhi(vec3 w){
    float sinTheta = SinTheta(w);
    return sinTheta == 0.0 ? 0.0 : clamp(w.y / sinTheta, -1.0, 1.0);
}
vec2 cx_mul(vec2 a, vec2 b){
    return vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x);
}
vec2 cx_div(vec2 a, vec2 b){
    return vec2((a.x*b.x+a.y*b.y) / (b.x*b.x+b.y*b.y), (a.y*b.x-a.x*b.y) / (b.x*b.x+b.y*b.y));
}
vec2 cx_sqrt(vec2 a) {
    float r = length(a);
    float rpart = SafeSqrt(0.5*(r+a.x));
    float ipart = SafeSqrt(0.5*(r-a.x));
    if (a.y < 0.0) ipart = -ipart;
    return vec2(rpart, ipart);
}
float BalanceHeuristic(uint nf, float pf, uint ng, float pg){
    return (nf * pf) / (nf * pf + ng * pg);
}