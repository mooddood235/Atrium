#include "BXDFHelpers.glsl"

BXDFSample SampleUberBSDF(vec3 wo, Material mat, float u0, float u1, float u2, float u3){
	BXDFSample bxdfSample;

	if (mat.metallic == 1.0 || u0 < mat.metallic){
		bxdfSample = SampleConductorBSDF(wo, mat, u1, u2);
		bxdfSample.pdf *= mat.metallic == 1.0 ? 1.0 : mat.metallic;
	}
	else{
		const float specChance = 0.5;
		if (mat.transmission > 0.0 || u1 < specChance){
			bxdfSample = SampleDielectricBSDF(wo, mat, u2, u3);
			bxdfSample.pdf *= mat.transmission > 0.0 ? 1.0 : specChance;
		}
		else{
			bxdfSample = SampleLambertianBRDF(wo, mat.albedo, u2, u3);
			bxdfSample.pdf *= 1.0 - specChance;
		}
		bxdfSample.pdf *= 1.0 - mat.metallic;
	}
	return bxdfSample;

}