#include "ImGui/imgui.h"
#include"ImGui/imgui_impl_opengl2.h"
#include "ImGui/imgui_impl_sdl.h"
#include <stdio.h>
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_opengl.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "GuiManager.h"
#include "ModuleRenderer3D.h"

GuiManager::GuiManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

GuiManager::~GuiManager()
{
}

bool GuiManager::Init()
{
	bool ret = true;
	ImGui::CreateContext();


	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();

	return true;
}

bool GuiManager::Start()
{
	
	return true;
}

update_status GuiManager::PreUpdate(float dt)
{
	
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	return UPDATE_CONTINUE;
}

update_status GuiManager::Update(float dt)
{
	
	return UPDATE_CONTINUE;
}

update_status GuiManager::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

bool GuiManager::CleanUp()
{
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_Quit();
	return true;
}
