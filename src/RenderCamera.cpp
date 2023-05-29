#include "RenderCamera.h"

using namespace Atrium;

RenderCamera::RenderCamera(
	const Camera& camera, unsigned int resolutionX, unsigned int resolutionY) {
	SetCamera(camera);
	film = Film(resolutionX, resolutionY);
	integrator = ShaderProgram("src/Shaders/Integrator.comp");
	environmentMap = Texture("EnvironmentMaps/White.png");
}
void RenderCamera::SetCamera(const Camera& camera) {
	modelMatrix = camera.GetTransform(Space::Global);
	projectionMatrix = camera.GetProjectionMatrix();
}
void RenderCamera::SetEnvironmentMap(const Texture& environmentMap) {
	this->environmentMap = environmentMap;
}
unsigned int RenderCamera::GetFilm() const{
	return film.GetTextureID();
}
void RenderCamera::Render() const{
	glm::uvec2 filmResolution = film.GetResolution();

	integrator.SetMat4("camera.modelMatrix", modelMatrix);
	integrator.SetMat4("camera.projectionMatrix", projectionMatrix);

	glBindImageTexture(0, film.GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, environmentMap.GetTextureID());

	integrator.Use();
	glDispatchCompute(filmResolution.x / 8, filmResolution.y / 4, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	integrator.UnUse();
}