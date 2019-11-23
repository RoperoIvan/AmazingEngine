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
	aspect_ratio = (float)16 / (float)9;
	int height;
	int width;
	SDL_GetWindowSize(App->window->window, &width, &height);
	frustum.verticalFov = frustum.nearPlaneDistance * tanf(90 * pi / 360.0f);
	frustum.horizontalFov = frustum.verticalFov * aspect_ratio;
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
	if ((App->motor_state == ENGINE_STATE::PLAY || App->motor_state == ENGINE_STATE::EXECUTE) && camera_active)
	{
		if (App->scene->current_camera != this)
		{
			App->scene->current_camera->camera_active = false;
			App->scene->current_camera = this;
		}
	}
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

	if (ImGui::SliderFloat("FOV", &fovindegrees, 1, 360))
	{
		frustum.verticalFov = frustum.nearPlaneDistance * tanf(fovindegrees * pi / 360.0f);
		frustum.horizontalFov = frustum.verticalFov * aspect_ratio;
	}
	ImGui::DragFloat("Near-Z Plane", &frustum.nearPlaneDistance, 0.1, 0.0, frustum.farPlaneDistance);
	ImGui::DragFloat("Far-Z Plane", &frustum.nearPlaneDistance, 0.1);

	if (ImGui::Checkbox("Game Camera", &camera_active))
		(&camera_active) ? true:false;

	
}
