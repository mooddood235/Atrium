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
bool IsSmooth(float a){
	return a <= 0.001;
}

float G1_ggx(vec3 w, float a);

float D_ggx(vec3 m, float a){
	return Sqr(a) / (PI * Sqr(Sqr(AbsCosTheta(m)) * (Sqr(a) - 1.0) + 1.0));
}
float VNDF_ggx(vec3 w, vec3 m, float a){
	return G1_ggx(w, a) / AbsCosTheta(w) * D_ggx(m, a) * AbsDot(w, m);
}
float Lambda(vec3 w, float a){
	float tan2Theta = Tan2Theta(w);
	if (isinf(tan2Theta) || isnan(tan2Theta)) return 0.0;
	return (sqrt(1.0 + Sqr(a) * tan2Theta) - 1.0) / 2.0;
}
float G1_ggx(vec3 w, float a){
	return 1.0 / (1.0 + Lambda(w, a));
}
float G_ggx(vec3 wo, vec3 wi, float a){
	return 1.0 / (1.0 + Lambda(wo, a) + Lambda(wi, a));
}
vec3 SampleGGX(vec3 w, float a, float u0, float u1){
	// Section 3.2: transforming the view direction to the hemisphere configuration
	vec3 Vh = normalize(vec3(a * w.x, a * w.y, w.z));
	// Section 4.1: orthonormal basis (with special case if cross product is zero)
	float lensq = Vh.x * Vh.x + Vh.y * Vh.y;
	vec3 T1 = lensq > 0 ? vec3(-Vh.y, Vh.x, 0) * inversesqrt(lensq) : vec3(1,0,0);
	vec3 T2 = cross(Vh, T1);
	// Section 4.2: parameterization of the projected area
	float r = sqrt(u0);
	float phi = 2.0 * PI * u1;
	float t1 = r * cos(phi);
	float t2 = r * sin(phi);
	float s = 0.5 * (1.0 + Vh.z);
	t2 = (1.0 - s)*sqrt(max(0.0, 1.0 - t1*t1)) + s*t2;
	// Section 4.3: reprojection onto hemisphere
	vec3 Nh = t1*T1 + t2*T2 + sqrt(max(0.0, 1.0 - t1*t1 - t2*t2))*Vh;
	// Section 3.4: transforming the normal back to the ellipsoid configuration
	vec3 Ne = normalize(vec3(a * Nh.x, a * Nh.y, max(0.0, Nh.z)));
	return Ne;
}
float pdf_ggx(vec3 wo, vec3 m, float a){
	return VNDF_ggx(wo, m, a) / (4.0 * AbsDot(wo, m));
}
