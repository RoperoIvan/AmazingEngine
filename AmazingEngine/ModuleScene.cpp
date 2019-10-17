#include "ModuleScene.h"
#include "ImGui/imgui.h"
#include"ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

ModuleScene::ModuleScene(Application* app, bool start_enable): Module(app)
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	
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
	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
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

	return true;
}
