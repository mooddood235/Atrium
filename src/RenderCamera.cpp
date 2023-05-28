#include "RenderCamera.h"

using namespace Atrium;

RenderCamera::RenderCamera(const Camera& camera, unsigned int resolutionX, unsigned int resolutionY) {
	SetCamera(camera);
	film = Film(resolutionX, resolutionY);
	integrator = ShaderProgram("src/Shaders/Integrator.comp");
}
void RenderCamera::SetCamera(const Camera& camera) {
	modelMatrix = camera.GetTransform(Space::Global);
	projectionMatrix = camera.GetProjectionMatrix();
}
unsigned int RenderCamera::GetFilm() {
	return film.GetTextureID();
}
void RenderCamera::Render() {
	glm::uvec2 filmResolution = film.GetResolution();

	integrator.SetMat4("camera.modelMatrix", modelMatrix);
	integrator.SetMat4("camera.projectionMatrix", projectionMatrix);

	glBindImageTexture(0, film.GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	integrator.Use();
	glDispatchCompute(filmResolution.x / 8, filmResolution.y / 4, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	integrator.UnUse();
}