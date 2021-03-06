#include "ModuleScene.h"
#include "ImGui/imgui.h"
#include"ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "FileSystem.h"
#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/Debug/MathGeoLib.lib")
#else
#pragma comment (lib, "MathGeoLib/libx86/Release/MathGeoLib.lib")
#endif

ModuleScene::ModuleScene(Application* app, bool start_enable): Module(app)
{

}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	App->camera->my_camera = new Camera(nullptr);
	current_camera = App->camera->my_camera;

	float3 aux[8] = { float3(-100,-100,-100),float3(-100,-100,100), float3(-100,100,-100), float3(-100,100,100), float3(100,-100,-100), float3(100,-100,100), float3(100,100,-100), float3(100,100,100) };
	AABB first;
	first.SetNegativeInfinity();
	first.Enclose(&aux[0], 8);
	octree = new Octree(first, 4, 3, 1);
	return true;
}


bool ModuleScene::Start()
{
	App->camera->my_camera->frustum.pos = { 3,3,3 };
	App->camera->my_camera->Look(float3::zero);
	App->mesh->LoadFile("..\\Assets\\street\\Street environment_V01.fbx");
	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	for (std::vector<GameObject*>::iterator object = game_objects.begin(); object != game_objects.end(); ++object)
	{
		if ((*object)->to_delete)
		{
			if(*object == game_object_select)
				game_object_select = nullptr;
			octree->Remove(*object);
			delete(*object);
			(*object) = nullptr;
			object = game_objects.erase(object);
			break;
		}
		if (game_object_select != *object)
			(*object)->show_inspector_window = false;
	}
	
	return UPDATE_CONTINUE;
}

update_status ModuleScene::GameUpdate(float game_dt)
{
	//frustrum and octree actived
	Timer frustrumTime;
	frustrumTime.Start();

	std::vector<MouseHit> draw_objects;
	App->scene->octree->CollectObjects(App->camera->my_camera->frustum, draw_objects);

	for (std::vector<MouseHit>::iterator iter = draw_objects.begin(); iter != draw_objects.end(); ++iter)
	{
		(*iter).object->Draw();
	}
	draw_objects.clear();
	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
	{
		float time = frustrumTime.Read();
		LOG("Frustrum time with octree: %f", time);
	}
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{		
	
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
	{
		if (game_object_select != nullptr)
			App->camera->GoAroundGeometry(game_object_select);
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		char* path = "Scene1231";	
		App->mesh->SaveCurrentScene(path);
	}
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		char* path = "Scene1231";
		App->mesh->LoadSceneFromFormat(path);
	}
	DrawPlane();

	if(game_object_select != nullptr)
		if (game_object_select->show_inspector_window == false)
			game_object_select = nullptr;
	if (App->guiManager->frustum_culling)
	{

		Timer frustrumTime;
		frustrumTime.Start();

		if (App->guiManager->active_octree)
		{
			std::vector<MouseHit> draw_objects;
			App->scene->octree->CollectObjects(App->camera->my_camera->frustum, draw_objects);

			for (std::vector<MouseHit>::iterator iter = draw_objects.begin(); iter != draw_objects.end(); ++iter)
			{
				(*iter).object->Draw();
			}
			draw_objects.clear();
			if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
			{
				float time = frustrumTime.Read();
				LOG("Frustrum time with octree: %f", time);
			}
		}
		else
		{
			for (std::vector<GameObject*>::iterator iter = game_objects.begin(); iter != game_objects.end(); ++iter)
			{
				(*iter)->Draw();
			}
			if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
			{
				float time = frustrumTime.Read();
				LOG("Frustrum time without octree: %f", time);
			}

		}
	}
	else
		for (std::vector<GameObject*>::iterator iter = game_objects.begin(); iter != game_objects.end(); ++iter)
		{
			(*iter)->Draw();
		}

	return UPDATE_CONTINUE;
}

void ModuleScene::DrawPlane()
{
	//Draw plane
	int num_lines = 100;
	glEnableClientState(GL_VERTEX_ARRAY);
	for (int j = 0; j < num_lines; ++j)
	{
		glColor3f(255.f, 255.f, 255.f);
		glBegin(GL_LINES);
		glVertex3f(j, 0, -num_lines);
		glVertex3f(j, 0, num_lines);

		glVertex3f(-j, 0, -num_lines);
		glVertex3f(-j, 0, num_lines);

		glVertex3f(-num_lines, 0, -j);
		glVertex3f(num_lines, 0, -j);

		glVertex3f(-num_lines, 0, j);
		glVertex3f(num_lines, 0, j);
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

update_status ModuleScene::PostUpdate(float dt)
{
	//game_object_select = nullptr;
	for (std::vector<GameObject*>::iterator object = game_objects.begin(); object != game_objects.end(); ++object)
	{
		(*object)->Update();
	}

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	for (std::vector<GameObject*>::iterator it = game_objects.begin(); it != game_objects.end(); ++it)
	{
		if ((*it) != nullptr)
			delete (*it);
		(*it) = nullptr;
	}
	game_objects.clear();

	for (std::vector<Image*>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		if ((*it) != nullptr)
		{
			delete (*it);
			(*it) = nullptr;
		}
	}
	textures.clear();

	octree->Clear();
	octree = nullptr;

	return true;
}

void ModuleScene::DeleteTexture(Image* tex)
{
	for (std::vector<Image*>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		if (tex == *it)
		{
			textures.erase(it);
			break;
		}
	}
}

void ModuleScene::RemoveSceneContent()
{
	App->scene->game_object_select = nullptr;
	CleanUp();
	float3 aux[8] = { float3(-100,-100,-100),float3(-100,-100,100), float3(-100,100,-100), float3(-100,100,100), float3(100,-100,-100), float3(100,-100,100), float3(100,100,-100), float3(100,100,100) };
	AABB first;
	first.Enclose(&aux[0], 8);
	octree = new Octree(first, 2,4,1);
}


GameObject* ModuleScene::CreateCamera()
{
	GameObject* camera = new GameObject();
	Geometry* mesh = (Geometry*)camera->CreateComponent(COMPONENT_TYPE(COMPONENT_TYPE::COMPONENT_MESH));
	(Transform*)camera->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Camera* cam = (Camera*)camera->CreateComponent(COMPONENT_TYPE::COMPONENT_CAMERA);
	game_objects.push_back(camera);
	if (current_camera == App->camera->my_camera)
		cam->camera_active = true;
	return camera;
}