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
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();
	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
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
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
