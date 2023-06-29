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
//	vec3 wi = vec3(-wo.x, -wo.y, wo.z);
//	vec3 f = FSchlick(AbsCosTheta(wi), albedo) / AbsCosTheta(wi); 
//	float pdf = 1.0;
//
//	return BXDFSample(f, pdf, wi);
    
    if (mat.roughness <= 0.001) {
        // Sample perfect specular conductor BRDF
        vec3 wi = vec3(-wo.x, -wo.y, wo.z);
        vec3 f = FSchlick(AbsCosTheta(wi), mat.albedo) / AbsCosTheta(wi); 
        float pdf = 1.0;
        return BXDFSample(f, pdf, wi);
    }
    // Sample rough conductor BRDF
    // Sample microfacet normal $\wm$ and reflected direction $\wi$
    if (wo.z == 0.0)
        return InvalidSample;
    vec3 wm = SampleVNDF_ggx(wo, mat.roughness, u0, u1);
    vec3 wi = reflect(-wo, wm);
//    if (!SameHemisphere(wo, wi))
//        return InvalidSample;

    // Compute PDF of _wi_ for microfacet reflection
    float pdf = VNDF_ggx(wo, wm, mat.roughness) / (4.0 * AbsDot(wo, wm));

    float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
    if (cosTheta_i == 0.0 || cosTheta_o == 0.0)
        return InvalidSample;
    // Evaluate Fresnel factor _F_ for conductor BRDF
    vec3 F = FSchlick(AbsCosTheta(wi), mat.albedo) / AbsCosTheta(wi); 

    vec3 f =
        F / max(EPSILON, 4.0 * cosTheta_i * cosTheta_o);
    f = F;
    return BXDFSample(f, pdf, wi);
}