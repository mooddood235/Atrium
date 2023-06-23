#include "Math.glsl"
#include "Samplers.glsl"
#include "Material.glsl"

float G1_ggx(vec3 w, float a);

float D_ggx(vec3 m, float a){
	return Sqr(a) / (PI * Sqr(Sqr(SCosTheta(m)) * (Sqr(a) - 1.0) + 1.0));
}
float VNDF_ggx(vec3 w, vec3 m, float a){
	return G1_ggx(w, a) / SCosTheta(w) * D_ggx(m, a) * sdot(w, m);
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
vec3 Sample_ggx_m(vec3 w, float a, float u0, float u1){
	vec3 v = w;
	//Section 3.2: transforming the view direction to the hemisphere configuration
	vec3 Vh = normalize(vec3(a * v.x, a * v.y, v.z));
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
	t2 = (1.0 - s)*sqrt(1.0 - t1*t1) + s*t2;
	// Section 4.3: reprojection onto hemisphere
	vec3 Nh = t1*T1 + t2*T2 + sqrt(max(0.0, 1.0 - t1*t1 - t2*t2))*Vh;
	// Section 3.4: transforming the normal back to the ellipsoid configuration
	vec3 Ne = normalize(vec3(a * Nh.x, a * Nh.y, max(0.0, Nh.z)));

	return Ne;
}

vec3 f_ggx(vec3 wo, vec3 wi, vec3 m, float a, Material mat){
	vec3 F0 = mix(vec3(0.04), mat.albedo, mat.metallic);
    vec3 F = F(wo, m, F0);
	return D_ggx(m, a) * F * G_ggx(wo, wi, a) / max(EPSILON * 100.0, 4.0 * SCosTheta(wi) * SCosTheta(wo));
}
float p_ggx(vec3 wo, vec3 m, float a){
	return max(EPSILON, VNDF_ggx(wo, m, a)) / (4.0 * sdot(wo, m));
}
vec3 Sample_ggx(vec3 wo, float a, Material mat, float u0, float u1, out vec3 wi, out vec3 m, out float pdf){
	m = Sample_ggx_m(wo, a, u0, u1);
	wi = reflect(-wo, m);
	pdf = p_ggx(wo, m, a);
	return f_ggx(wo, wi, m, a, mat);
}