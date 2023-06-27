#include "BXDFHelpers.glsl"

vec3 f_DielectricBSDF(vec3 wo, vec3 wi, float eta){
	if (eta == 1.0) return vec3(0.0);
	return vec3(0.0);
}
float pdf_DielectricBSDF(vec3 wo, vec3 wi, float eta){
	if (eta == 1.0) return 0.0;
	return 0.0;
}

BXDFSample SampleDielectricBSDF(vec3 wo, float eta, float u0, float u1) {
	float R = FrDielectric(CosTheta(wo), eta), T = 1.0 - R;
	float pr = R, pt = T;

	if (u0 < pr / (pr + pt)){
		vec3 wi = vec3(-wo.x, -wo.y, wo.z);
		vec3 f = vec3(R) / AbsCosTheta(wi);
		float pdf = pr / (pr + pt);
		return BXDFSample(f, pdf, wi);
	}
	else{
		vec3 wi;
		float etap;
		bool valid = Refract(wo, eta, etap, wi);
		if (!valid) return InvalidSample;
		vec3 f = vec3(T) / AbsCosTheta(wi);
		// TODO: Account for non-symmetry with transmission to different medium.
		float pdf = pt / (pr + pt);
		return BXDFSample(f, pdf, wi);
	}
	return BXDFSample(vec3(0), 1, vec3(0));
}