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