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

update_status ModuleCamera3D::PreUpdate(float dt)
{
	bool ret = true;
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && !write)
		CatchMousePicking();

	return ret ? UPDATE_CONTINUE : UPDATE_STOP;
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
	float current_speed = camera_speed;
	if (!write)
	{
		float3 newPos(0, 0, 0);
		//Movement controls
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			current_speed = 2.0f;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += c_frustum->front * current_speed; 
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= c_frustum->front * current_speed;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= c_frustum->WorldRight() * current_speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += c_frustum->WorldRight() * current_speed;
		
		// Mouse Zoom controls
		if (App->input->GetMouseZ() > 0)
		{
			newPos -= c_frustum->front * current_speed * 10;
		}
		if (App->input->GetMouseZ() < 0)
		{
			newPos += c_frustum->front * current_speed * 10;
		}
		
		
		
		// Mouse Rotation controls
		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();
			float3 newRot = my_camera->frustum.pos;
			float Sensitivity = 0.01f;
			if(dx != 0)
			{
				Quat Delta_x(my_camera->frustum.up, dx * Sensitivity);
				newRot = Delta_x.Transform(newRot);			
				my_camera->frustum.pos = newRot;
				my_camera->Look(my_camera->frustum.front);
			}
			if (dy != 0)
			{
				Quat Delta_y(my_camera->frustum.WorldRight(), dy * Sensitivity);
				newRot = Delta_y.Transform(newRot);
				my_camera->frustum.pos = newRot;
				my_camera->Look(my_camera->frustum.front);
			}
		}
		c_frustum->Translate(newPos);
		//Reference += newPos;
	}
}

void ModuleCamera3D::GoAroundGeometry(GameObject* obj)
{
	if (obj == nullptr)
		return;

	if (obj->bounding_box.IsFinite())
	{
		my_camera->Look(obj->bounding_box.CenterPoint());
		//Reference = obj->bounding_box.CenterPoint();
	}
}

void ModuleCamera3D::CatchMousePicking()
{
	float2 mouse_normal(App->input->GetMouseX(), App->input->GetMouseY());
	mouse_normal.x = -(1.0f - (float(mouse_normal.x) * 2.0f) / SCREEN_WIDTH);
	mouse_normal.y = 1.0f - (float(mouse_normal.y) * 2.0f) / SCREEN_HEIGHT;
	ray_picking = c_frustum->UnProjectLineSegment(mouse_normal.x, mouse_normal.y);

	float ray_direction = ray_picking.Length();
	GameObject* picked_obj = nullptr;

	std::vector<GameObject*>::iterator iter = App->scene->game_objects.begin();
	for (; iter != App->scene->game_objects.end(); ++iter)
	{
		(*iter)->LookForRayCollision(picked_obj, ray_picking, ray_direction);
	}

	if (picked_obj)
	{
		App->scene->game_object_select = picked_obj;
		App->scene->game_object_select->show_inspector_window = true;
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