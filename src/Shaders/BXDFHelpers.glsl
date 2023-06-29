#include "Math.glsl"

struct BXDFSample{
	vec3 f;
	float pdf;
	vec3 wi;
};

BXDFSample InvalidSample = BXDFSample(vec3(-1.0), 0.0, vec3(0.0));

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
vec2 SampleUniformDiskPolar(float u0, float u1) {
    float r = SafeSqrt(u0);
    float theta = 2.0 * PI * u1;
    return normalize(vec2(r * cos(theta), r * sin(theta)));
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
vec3 FSchlick(float cosTheta, vec3 F0){
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 F0ToIOR(vec3 F0){
	F0 = vec3(SafeSqrt(F0.r), SafeSqrt(F0.g), SafeSqrt(F0.b));
	F0 = min(vec3(0.9999), F0);
	return (1.0 + F0) / (1.0 - F0);
}
float D_ggx(vec3 m, float a){
	float tan2Theta = Tan2Theta(m);
    if (isinf(tan2Theta)) return 0.0;
    float cos4Theta = Sqr(Cos2Theta(m));
    if (cos4Theta < 1e-16f) return 0.0;
    float e = tan2Theta * (Sqr(CosPhi(m) / a) + Sqr(SinPhi(m) / a));
    return 1.0 / (PI * a * a * cos4Theta * Sqr(1.0 + e));
}
float Lambda(vec3 w, float a) {
    float tan2Theta = Tan2Theta(w);
    if (isinf(tan2Theta)) return 0.0;
    float alpha2 = Sqr(CosPhi(w) * a) + Sqr(SinPhi(w) * a);
    return (sqrt(1 + alpha2 * tan2Theta) - 1.0) / 2.0;
}
float G1(vec3 w, float a) { return 1.0 / (1.0 + Lambda(w, a)); }
float G(vec3 wo, vec3 wi, float a) { return 1.0 / (1.0 + Lambda(wo, a) + Lambda(wi, a)); }
float VNDF_ggx(vec3 w, vec3 m, float a) {
        return G1(w, a) / AbsCosTheta(w) * D_ggx(m, a) * AbsDot(w, m);
}
vec3 SampleVNDF_ggx(vec3 w, float a, float u0, float u1) {
        // Transform _w_ to hemispherical configuration
        vec3 wh = normalize(vec3(a * w.x, a * w.y, w.z));
        if (wh.z < 0.0) wh = -wh;

        // Find orthonormal basis for visible normal sampling
        vec3 T1 = (wh.z < 0.99999f) ? normalize(cross(vec3(0.0, 0.0, 1.0), wh))
                                        : vec3(1.0, 0.0, 0.0);
        vec3 T2 = cross(wh, T1);

        // Generate uniformly distributed points on the unit disk
        vec2 p = SampleUniformDiskPolar(u0, u1);

        // Warp hemispherical projection for visible normal sampling
        float h = sqrt(1.0 - Sqr(p.x));
        p.y = mix((1.0 + wh.z) / 2.0, h, p.y);

        // Reproject to hemisphere and transform normal to ellipsoid configuration
        float pz = sqrt(max(0.0, 1.0 - Sqr(length(p))));
        vec3 nh = p.x * T1 + p.y * T2 + pz * wh;
        //CHECK_RARE(1e-5f, nh.z == 0);
        return normalize(vec3(a * nh.x, a * nh.y, max(1e-6f, nh.z)));
    }