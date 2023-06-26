#include "BXDFHelpers.glsl"

vec3 f_ConductorBSDF(vec3 wo, vec3 wi){
	if (!SameHemisphere(wo, wi)) return vec3(0.0);
	return vec3(0.0);
}
vec3 pdf_ConductorBSDF(vec3 wo, vec3 wi){
	if (!SameHemisphere(wo, wi)) return vec3(0.0);
	return vec3(0.0);
}

BXDFSample SampleConductorBSDF(vec3 wo, vec2 eta){
	vec3 wi = vec3(-wo.x, -wo.y, wo.z);
	vec3 f = vec3(FrComplex(AbsCosTheta(wi), eta)) / AbsCosTheta(wi); 
	float pdf = 1.0;

	return BXDFSample(f, pdf, wi);
}