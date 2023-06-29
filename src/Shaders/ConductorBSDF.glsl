#include "BXDFHelpers.glsl"
#include "Material.glsl"

vec3 f_ConductorBSDF(vec3 wo, vec3 wi){
	if (!SameHemisphere(wo, wi)) return vec3(0.0);
	return vec3(0.0);
}
vec3 pdf_ConductorBSDF(vec3 wo, vec3 wi){
	if (!SameHemisphere(wo, wi)) return vec3(0.0);
	return vec3(0.0);
}

BXDFSample SampleConductorBSDF(vec3 wo, Material mat, float u0, float u1){
    vec3 wi;
    vec3 f;
    float pdf;

    if (IsSmooth(mat.roughness)) {
        wi = vec3(-wo.x, -wo.y, wo.z);
        f = FSchlick(AbsCosTheta(wi), mat.albedo) / AbsCosTheta(wi); 
        pdf = 1.0;
    }
    else{
        vec3 m = SampleGGX(wo, mat.roughness, u0, u1);
        wi = reflect(-wo, m);

        if (!SameHemisphere(wo, wi)) return InvalidSample;

        vec3 F = FSchlick(AbsCosTheta(wi), mat.albedo);
        f = D_ggx(m, mat.roughness) * F * G_ggx(wo, wi, mat.roughness) / (4.0 * AbsCosTheta(wi) * AbsCosTheta(wo));
        pdf = pdf_ggx(wo, m, mat.roughness);
    }
    return BXDFSample(f, pdf, wi);
}