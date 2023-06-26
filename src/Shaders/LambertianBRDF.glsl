#include "BXDFHelpers.glsl"
#include "Material.glsl"

vec3 f_LambertianBRDF(vec3 wo, vec3 wi, vec3 albedo){
	if (!SameHemisphere(wo, wi)) return vec3(0.0);
	return albedo / PI;
}
BXDFSample SampleLambertianBRDF(vec3 wo, vec3 albedo, float u0, float u1){
	vec3 wi = CosineSampleHemisphere(u0, u1);
	if (wo.z < 0) wi.z *= -1.0;
	float pdf = CosineSampleHemispherePDF(AbsCosTheta(wi));

	return BXDFSample(albedo / PI, pdf, wi);
}
float pdf_LambertianBRDF(vec3 wo, vec3 wi){
	if (!SameHemisphere(wo, wi)) return 0.0;
	return CosineSampleHemispherePDF(AbsCosTheta(wi));
}