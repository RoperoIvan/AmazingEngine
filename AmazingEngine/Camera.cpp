#include "Camera.h"
#include "Application.h"
#include "ImGui/imgui.h"

Camera::Camera(GameObject* parent) : Component(parent, COMPONENT_TYPE::COMPONENT_CAMERA)
{
	//parent->name = "Camera " + std::to_string(App->scene->game_objects.size());
	frustum.nearPlaneDistance = 1.f;
	frustum.farPlaneDistance = 10.f;
	window_aspect_ratio = App->window->current_aspect_ratio;
	frustum.type = PerspectiveFrustum;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;
	frustum.pos = float3::zero;
	frustum.verticalFov = 1.0;
	frustum.horizontalFov = atanf(tan(frustum.verticalFov * 0.5) * window_aspect_ratio) * 2;
}

Camera::~Camera()
{
}

void Camera::Disable()
{

}

void Camera::Update()
{
	window_aspect_ratio = App->window->current_aspect_ratio;
	App->mesh->AddFrustumBox(&frustum);
}

void Camera::Look(const float3 & Position)
{
	float3 future_pos = Position - frustum.pos;
	float3x3 dir_matrix = float3x3::LookAt(frustum.front, future_pos.Normalized(), frustum.up, float3::unitY);
	frustum.front = dir_matrix.MulDir(frustum.front).Normalized();
	frustum.up = dir_matrix.MulDir(frustum.up).Normalized();
}

float * Camera::GetViewMatrix()
{
	return (float*)static_cast<float4x4>(frustum.ViewMatrix()).Transposed().v;;
}
