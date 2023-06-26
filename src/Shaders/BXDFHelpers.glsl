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
bool Refract(vec3 wi, float eta, out float etap, out vec3 wt){
	vec3 n = vec3(0.0, 0.0, 1.0);
	float cosTheta_i = CosTheta(wi);

	if (cosTheta_i < 0.0){
		eta = 1.0 / eta;
		cosTheta_i = -cosTheta_i;
		n = -n;
	}
	float sin2Theta_i = max(0.0, 1.0 - Sqr(cosTheta_i));
	float sin2Theta_t = sin2Theta_i / Sqr(eta);
	if (sin2Theta_t >= 1.0){
		return false;
	}
	float cosTheta_t = SafeSqrt(1.0 - sin2Theta_t);

	wt = -wi / eta + (cosTheta_i / eta - cosTheta_t) * n;

	etap = eta;
	return true;
}
float FrDielectric(float cosTheta_i, float eta){
	cosTheta_i = clamp(cosTheta_i, -1.0, 1.0);
    // Potentially flip interface orientation for Fresnel equations
    if (cosTheta_i < 0.0) {
        eta = 1.0 / eta;
        cosTheta_i = -cosTheta_i;
    }
    // Compute $\cos\,\theta_\roman{t}$ for Fresnel equations using Snell's law
    float sin2Theta_i = 1.0 - Sqr(cosTheta_i);
    float sin2Theta_t = sin2Theta_i / Sqr(eta);
    if (sin2Theta_t >= 1.0)
        return 1.0;
    float cosTheta_t = SafeSqrt(1.0 - sin2Theta_t);

    float r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
    float r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
    return (Sqr(r_parl) + Sqr(r_perp)) / 2.0;
};
float FrComplex(float cosTheta_i, vec2 eta){
    cosTheta_i = clamp(cosTheta_i, 0.0, 1.0);
    // Compute complex $\cos\,\theta_\roman{t}$ for Fresnel equations using Snell's law
    float sin2Theta_i = 1.0 - Sqr(cosTheta_i);
    vec2 sin2Theta_t = cx_div(vec2(sin2Theta_i), cx_mul(eta, eta));
    vec2 cosTheta_t = cx_sqrt(1.0 - sin2Theta_t);

    vec2 r_parl = cx_div(eta * cosTheta_i - cosTheta_t, eta * cosTheta_i + cosTheta_t);
    vec2 r_perp = cx_div(cosTheta_i - eta * cosTheta_t, cosTheta_i + eta * cosTheta_t);
    return (Sqr(length(r_parl)) + Sqr(length(r_perp))) / 2.0;
}
