#include "BXDFHelpers.glsl"
#include "Material.glsl"

vec3 f_DielectricBSDF(vec3 wo, vec3 wi, float eta){
	if (eta == 1.0) return vec3(0.0);
	return vec3(0.0);
}
float pdf_DielectricBSDF(vec3 wo, vec3 wi, float eta){
	if (eta == 1.0) return 0.0;
	return 0.0;
}

BXDFSample SampleDielectricBSDF(vec3 wo, Material mat, float u0, float u1) {
	vec3 wi;
	vec3 f;
	float pdf;

	if (IsSmooth(mat.roughness)){
		float R = FrDielectric(CosTheta(wo), mat.ior), T = 1.0 - R;
		float pr = R, pt = T;

		if (mat.transmission == 0.0 || u0 < pr / (pr + pt)){
			wi = vec3(-wo.x, -wo.y, wo.z);
			f = vec3(R) / AbsCosTheta(wi);
			pdf = mat.transmission == 0.0 ? 1.0 : pr / (pr + pt);
		}
		else{
			float etap;
			bool valid = Refract(wo, vec3(0.0, 0.0, 1.0), mat.ior, etap, wi);
			if (!valid) return InvalidSample;

			f = vec3(T) / AbsCosTheta(wi);
			// TODO: Account for non-symmetry with transmission to different medium.
			pdf = pt / (pr + pt);
		}
	}
	else{
		vec3 m = SampleGGX(wo, mat.roughness, u0, u1);
		float R = FrDielectric(dot(wo, m), mat.ior), T = 1.0 - R;
		float pr = R, pt = T;

		if (true){
			// Sample reflection at rough dielectric interface
			wi = reflect(-wo, m);
			if (!SameHemisphere(wo, wi)) return InvalidSample;

			// Compute PDF of rough dielectric reflection
			pdf = pdf_ggx(wo, m, mat.roughness) * 1.0;
			f = D_ggx(m, mat.roughness) * vec3(R) * G_ggx(wo, wi, mat.roughness) / (4.0 * AbsCosTheta(wi) * AbsCosTheta(wo));
		}
		//TODO: Add rough transmission
	}
	return BXDFSample(f, pdf, wi);
}