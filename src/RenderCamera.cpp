#include "RenderCamera.h"

using namespace Atrium;

void RenderCamera::Init() {
	integrator = ShaderProgram("src/Shaders/Integrator.comp");
}
void RenderCamera::Render(const Film& film, const Camera& camera, const AStructure& aStructure, const Texture& environmentMap) {
	integrator.SetMat4("camera.modelMatrix", camera.GetTransform(Space::Global).GetMatrix());
	integrator.SetMat4("camera.projectionMatrix", camera.GetProjectionMatrix());

	aStructure.Bind();

	glBindImageTexture(0, film.GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, environmentMap.GetTextureID());
	
	integrator.Use();
	glDispatchCompute(film.GetResolution().x / 8, film.GetResolution().y / 4, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	integrator.UnUse();
}

ShaderProgram RenderCamera::integrator = ShaderProgram();