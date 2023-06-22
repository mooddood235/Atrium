#include "Math.glsl"
#include "Material.glsl"

vec3 f_lambert(vec3 wo, vec3 m, Material mat){
	vec3 F0 = mix(vec3(0.04), mat.albedo, mat.metallic);
    vec3 F = F(wo, m, F0);
	return (1.0 - mat.metallic) * (1.0 - F) * mat.albedo / PI;
}