#include "Camera.h"
#include "Application.h"
#include "ImGui/imgui.h"

Camera::Camera(GameObject* parent, float z_near_distance, float z_far_distance) : Component(parent, COMPONENT_TYPE::COMPONENT_CAMERA)
{
	frustum.nearPlaneDistance = z_near_distance;
	frustum.farPlaneDistance = z_far_distance;
	window_aspect_ratio = App->window->current_aspect_ratio;
	frustum.type = PerspectiveFrustum;
	frustum.front = float3(0, 0, 1);
	frustum.up = float3(0, 1, 0);
	frustum.pos = float3(0, 2, -8);
	frustum.verticalFov = 1.0;
	frustum.horizontalFov = math::Atan(window_aspect_ratio*math::Tan(frustum.verticalFov / 2)) * 2;

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

