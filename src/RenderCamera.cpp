#include "RenderCamera.h"
#include <iostream>

using namespace Atrium;

void RenderCamera::Init() {
	integrator = ShaderProgram("src/Shaders/Integrator.comp");
}
void RenderCamera::Render(
	const Film& film,
	const Camera& camera,
	const EnvironmentMap& environmentMap,
	unsigned int samples,
	unsigned int depth,
	unsigned int seed,
	RenderMode renderMode) {

	if (!sceneIsBound) {
		std::cout << "Render Error: you must bind a scene before rendering." << std::endl;
		exit(-1);
	}

	integrator.SetUint("seed", seed);
	integrator.SetUint("samples", samples);
	integrator.SetUint("depth", depth);
	integrator.SetUint("renderMode", (unsigned int)renderMode);
	integrator.SetMat4("camera.modelMatrix", camera.GetTransform(Space::Global).GetMatrix());
	integrator.SetMat4("camera.projectionMatrix", camera.GetProjectionMatrix());

	glBindImageTexture(0, film.GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, environmentMap.GetMapID());
	integrator.SetUint("envMapWidth", environmentMap.GetDims().x);
	integrator.SetUint("envMapHeight", environmentMap.GetDims().y);
	environmentMap.BindBins();

	integrator.Use();
	glDispatchCompute(film.GetResolution().x / 8, film.GetResolution().y / 4, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	integrator.UnUse();
}

void RenderCamera::BindScene(const Scene& scene){
	scene.bvh.Bind();
	scene.bvh.MakeTextureHandlesResident();
	sceneIsBound = true;
}

void RenderCamera::UnBindScene(const Scene& scene){
	scene.bvh.MakeTextureHandlesNonResident();
	sceneIsBound = false;
}

ShaderProgram RenderCamera::integrator = ShaderProgram();
bool RenderCamera::sceneIsBound = false;