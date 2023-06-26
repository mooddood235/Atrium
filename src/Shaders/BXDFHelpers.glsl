#include "Math.glsl"

struct BXDFSample{
	vec3 f;
	float pdf;
	vec3 wi;
};

bool SameHemisphere(vec3 w0, vec3 w1){
	return w0.z * w1.z > 0;
}
vec3 SampleHemisphere(float u0, float u1){
	float theta = SafeACos(u0);
	float phi = 2.0 * PI * u1;

	return normalize(vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta)));
}
vec3 CosineSampleHemisphere(float u0, float u1){
	float theta = SafeACos(SafeSqrt(u0));
	float phi = 2.0 * PI * u1;

	return normalize(vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta)));
}
float SampleHemispherePDF(){
	return 1.0 / (2.0 * PI);
}
float CosineSampleHemispherePDF(float cosTheta){
	return cosTheta / PI;
}