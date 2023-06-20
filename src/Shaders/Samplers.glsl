#include "Math.glsl"

vec3 SampleHemisphere(float u0, float u1) {
    float z = u0;
    float r = sqrt(max(0.0, 1.0 - z * z));
    float phi = 2.0 * PI * u1;
    return vec3(r * cos(phi), r * sin(phi), z);
}
float SampleHemispherePDF(){
    return 1.0 / (2.0 * PI);
}
vec2 SampleDiskPolar(float u0, float u1){
	float r = sqrt(u0);
	float theta = 2.0 * PI * u1;
	return vec2(r * cos(theta), r * sin(theta));
}
float Halton(uint base, uint index){
	float result = 0.0;
	float f = 1.0;
	while (index > 0){
		f = f / float(base);
		result += f * float(index % base);
		index = index / base; 
	}
	return result;
}
