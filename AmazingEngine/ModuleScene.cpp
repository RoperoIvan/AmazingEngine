#include "ModuleScene.h"
#include "ImGui/imgui.h"
#include"ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include <fstream>
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
	GameObject* test_camera = new GameObject();
	game_objects.push_back(test_camera);
	test_camera->CreateCamera(0.1, 10);

	return true;
}

bool ModuleScene::Start()
{
	App->mesh->LoadFile("..\\Assets\\BakerHouse.fbx");
	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{

	for (std::vector<GameObject*>::iterator object = game_objects.begin(); object != game_objects.end(); ++object)
	{
		if ((*object)->to_delete)
		{
			game_objects.erase(object);
			break;
		}
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
		char* path = "Scene1231.Amazing";	
		SaveScene(path);
	}
	DrawPlane();

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
	game_object_select = nullptr;
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

bool ModuleScene::SaveScene(char* path)
{
	bool ret = true;
	FILE* file;
	file = std::fopen(path, "wt");

	if (file == NULL)
	{
		SDL_GetError();
		return false;
	}
	for (std::vector<GameObject*>::iterator object = game_objects.begin(); object != game_objects.end(); ++object)
	{
		if (*object != nullptr)
		{
			(*object)->SaveMesh(file);
		}
	}
	std::fclose(file);
	return ret;
}

