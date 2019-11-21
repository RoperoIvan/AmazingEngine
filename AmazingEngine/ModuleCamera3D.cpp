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
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !write && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemFocused() && !ImGui::IsAnyWindowFocused() && !ImGui::IsAnyWindowHovered())
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
	CameraControls(dt);
	c_frustum = &my_camera->frustum;

	return UPDATE_CONTINUE;
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

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += my_camera->frustum.front * current_speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= my_camera->frustum.front * current_speed;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= my_camera->frustum.WorldRight() * current_speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += my_camera->frustum.WorldRight() * current_speed;
		
		// Mouse Zoom controls
		if (App->input->GetMouseZ() > 0)
		{
			newPos -= my_camera->frustum.front * current_speed * 10;
		}
		if (App->input->GetMouseZ() < 0)
		{
			newPos += my_camera->frustum.front * current_speed * 10;
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
		my_camera->frustum.Translate(newPos);
		//Reference += newPos;
	}
}

void ModuleCamera3D::GoAroundGeometry(GameObject* obj)
{
	if (obj == nullptr)
		return;

	if (obj->bounding_box->aabb.IsFinite())
	{
		my_camera->Look(obj->bounding_box->aabb.CenterPoint());
		//Reference = obj->bounding_box.CenterPoint();
	}
}

void ModuleCamera3D::CatchMousePicking()
{
	//Normalize the mouse position
	int height;
	int width;
	SDL_GetWindowSize(App->window->window, &width, &height);
	float2 mouse_normal;
	mouse_normal.x = -1.0 + 2.0 * App->input->GetMouseX() / width;
	mouse_normal.y = 1.0 - 2.0 * App->input->GetMouseY() / height;

	//Create the list of distances and objects that we will use to select the closest gameobject that was hit
	std::vector<MouseHit> hit;

	//Pass the mouse position into the  ray projection of the camera frustum
	Timer whithoutOctree;
	whithoutOctree.Start();
	ray_picking = my_camera->frustum.UnProjectLineSegment(mouse_normal.x, mouse_normal.y);

	if (!App->guiManager->active_octree)
	{
		for (std::vector<GameObject*>::iterator iter = App->scene->game_objects.begin(); iter != App->scene->game_objects.end(); ++iter)
		{
			(*iter)->LookForRayCollision(ray_picking, hit);
		}
		LOG("Collect ray objects without octree %f", whithoutOctree.Read());
		////with octree
	}
	else
	{
	
	std::vector<GameObject*>objects_picked;
	App->scene->octree->CollectObjects(ray_picking, objects_picked);
	for (std::vector<GameObject*>::iterator iter = objects_picked.begin(); iter != objects_picked.end(); ++iter)
	{
		(*iter)->LookForRayCollision(ray_picking, hit);
	}
	LOG("Collect ray objects with octree %f", whithoutOctree.Read());
	}
	if (!hit.empty())
	{
		//We order all the hits from closer to furthest
		std::sort(hit.begin(), hit.end(), less_than_key());
		std::vector<MouseHit>::iterator it = hit.begin();
		if ((*it).object)
		{
			//Assign the closest one to object_selector
			App->scene->game_object_select = (*it).object;
			App->scene->game_object_select->show_inspector_window = true;
		}
		hit.clear();
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