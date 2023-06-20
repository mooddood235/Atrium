#include "Math.glsl"
#include "Samplers.glsl"

float G1(vec3 w, float a);

float D(vec3 m, float a){
	return Sqr(a) / (PI * Sqr(Sqr(SCosTheta(m)) * (Sqr(a) - 1.0) + 1.0));
}
float VNDF(vec3 w, vec3 m, float a){
	return G1(w, a) / SCosTheta(w) * D(m, a) * sdot(w, m);
}
float Lambda(vec3 w, float a){
	float tan2Theta = Tan2Theta(w);
	if (isinf(tan2Theta) || isnan(tan2Theta)) return 0.0;
	return (sqrt(1.0 + Sqr(a) * tan2Theta) - 1.0) / 2.0;
}
float G1(vec3 w, float a){
	return 1.0 / (1.0 + Lambda(w, a));
}
float G(vec3 wo, vec3 wi, float a){
	return 1.0 / (1.0 + Lambda(wo, a) + Lambda(wi, a));
}
vec3 F(vec3 v, vec3 h, vec3 F0){
	return F0 + (1.0 - F0) * pow((1.0 - sdot(v, h)), 5.0);
}
vec3 Sample_m(vec3 w, float a, float u0, float u1){
//	vec3 wh = normalize(vec3(a * w.x, a * w.y, w.z));
//	if (wh.z < 0) wh = -wh;
//
//	vec3 T1 = (wh.z < 0.99999) ? normalize(cross(vec3(0.0, 0.0, 1.0), wh)) : vec3(1.0, 0.0, 0.0);
//	vec3 T2 = cross(wh, T1);
//
//	vec2 p = SampleDiskPolar(u0, u1);
//
//	float h = sqrt(1.0 - Sqr(p.x));
//	p.y = mix((1.0 + wh.z) / 2.0, h, p.y);
//
//	float pz = sqrt(max(0.0, 1.0 - Sqr(length(p))));
//	vec3 nh = p.x * T1 + p.y * T2 + pz * wh;
//	
//	return normalize(vec3(nh.x * a, nh.y * a, max(EPSILON, nh.z)));
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