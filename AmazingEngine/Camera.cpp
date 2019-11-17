#include "Camera.h"
#include "Application.h"
#include "ImGui/imgui.h"

Camera::Camera(GameObject* parent) : Component(parent, COMPONENT_TYPE::COMPONENT_CAMERA)
{
	//parent->name = "Camera " + std::to_string(App->scene->game_objects.size());
	frustum.nearPlaneDistance = 1.f;
	frustum.farPlaneDistance = 30.f;
	frustum.type = PerspectiveFrustum;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;
	frustum.pos = float3::zero;
	frustum.verticalFov = 1.0;
	frustum.horizontalFov = atanf(tan(frustum.verticalFov * 0.5) * ((float)16/(float)9)) * 2;
}

Camera::~Camera()
{
}

void Camera::Disable()
{

}

void Camera::Update()
{
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
	return (float*)static_cast<float4x4>(frustum.ViewMatrix()).Transposed().v;
}

void Camera::LoadCameraOptions()
{
	float z_near = 0.f;
	float z_far = 0.f;
	if (ImGui::DragFloat("FOV", &frustum.verticalFov, 0, 1, 1));
	{
		frustum.horizontalFov = atanf(tan(frustum.verticalFov * 0.5) * ((float)16 / (float)9)) * 2;
	}
	ImGui::DragFloat("Near-Z Plane", &frustum.nearPlaneDistance, 0.1, 0.0, frustum.farPlaneDistance);
	ImGui::DragFloat("Far-Z Plane", &frustum.nearPlaneDistance, 0.1);

}
