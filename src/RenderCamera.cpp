#include "RenderCamera.h"

using namespace Atrium;

RenderCamera::RenderCamera(const Camera& camera) {
	SetCamera(camera);
}
void RenderCamera::SetCamera(const Camera& camera) {
	modelMatrix = camera.GetTransform(Space::Global);
	projectionMatrix = camera.GetProjectionMatrix();
}