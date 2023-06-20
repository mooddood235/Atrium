#include "Math.glsl"

float G1(vec3 w, float a);

float D(vec3 m, float a){
	return Sqr(a) / (PI * Sqr(Sqr(CosTheta(m)) * (Sqr(a) - 1.0) + 1.0));
}
float VNDF(vec3 w, vec3 m, float a){
	return G1(w, a) / AbsCosTheta(w) * D(m, a) * sdot(w, m);
}
float Lambda(vec3 w, float a){
	return (sqrt(1.0 + Sqr(a) * Tan2Theta(w)) - 1.0) / 2.0;
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
	vec3 wh = normalize(vec3(a * w.x, a * w.y, w.z));
	if (wh.z < 0) wh = -wh;

	return vec3(0);
}