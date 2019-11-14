#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "Geometry.h"
#include "Transform.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleCamera3D::~ModuleCamera3D()
{
	delete my_camera;
	my_camera = nullptr;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	c_frustum = &my_camera->frustum;
	CameraControls(dt);

	return UPDATE_CONTINUE;
}
void ModuleCamera3D::Move(const float3& Movement)
{
	c_frustum->Translate(Movement);
}

void ModuleCamera3D::CameraControls(float dt)
{
	if (!write)
	{
		float3 newPos(0, 0, 0);
		//Movement controls
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed = 8.0f;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += c_frustum->front * speed; Reference += newPos * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= c_frustum->front * speed; Reference += newPos * speed;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= c_frustum->WorldRight() * speed; Reference += newPos * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += c_frustum->WorldRight() * speed; Reference += newPos * speed;
		
		// Mouse Zoom controls
		if (App->input->GetMouseZ() > 0)
		{
			newPos -= c_frustum->front * speed * 10;
		}
		if (App->input->GetMouseZ() < 0)
		{
			newPos += c_frustum->front * speed * 10;
		}
		
		
		// Mouse Rotation controls
		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		{
			float3 newRot(0, 0, 0);

			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();
			newRot = my_camera->frustum.pos - Reference;
			float Sensitivity = 0.01f;
			if(dx != 0 && dy != 0)
			{
				Quat Delta_x(my_camera->frustum.up, dx * Sensitivity);
				Quat Delta_y(my_camera->frustum.WorldRight(), dy * Sensitivity);
				newRot = Delta_x.Transform(newRot);
				newRot = Delta_y.Transform(newRot);
				my_camera->frustum.pos = newRot - Reference;
				my_camera->Look(Reference);
			}
		}
		c_frustum->Translate(newPos);
	}
}

void ModuleCamera3D::GoAroundGeometry(GameObject* obj)
{
	if (obj == nullptr)
		return;

	if (obj->bounding_box.IsFinite())
	{
		my_camera->Look(obj->bounding_box.CenterPoint());
		Reference = obj->bounding_box.CenterPoint();
	}
}
bool ModuleCamera3D::Save(nlohmann::json& j) const
{
	return true;
}

bool ModuleCamera3D::Load(nlohmann::json& j)
{
	return true;
}