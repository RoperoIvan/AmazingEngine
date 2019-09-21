#include "ImGui/imgui.h"
#include"ImGui/imgui_impl_opengl3.h"
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
	ImGui_ImplOpenGL3_Init();

	show_demo_window = false;
	return true;
}

bool GuiManager::Start()
{
	
	return true;
}

update_status GuiManager::PreUpdate(float dt)
{
	bool ret = true;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Example Window")) show_demo_window = true;
			if (ImGui::MenuItem("Exit", "exit", false)) ret = false;

			ImGui::EndMenu();
		}	
		ImGui::EndMainMenuBar();
	}
	if(show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	return ret ? UPDATE_CONTINUE : UPDATE_STOP;
}

update_status GuiManager::Update(float dt)
{
	CollisionsBetweenObjects();
	return UPDATE_CONTINUE;
}

update_status GuiManager::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

bool GuiManager::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(App->renderer3D->context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();
	return true;
}

void GuiManager::CollisionsBetweenObjects()
{
	LCG rand;
	float sph_posx = rand.Float(0, 20);
	float sph_posy = rand.Float(0, 20);
	float sph_posz = rand.Float(0, 20);
	sph.r = rand.Float(0, 10);
	sph.pos.Set(sph_posx, sph_posy, sph_posz);

	float cap_posx = rand.Float(0, 20);
	float cap_posy = rand.Float(0, 20);
	float cap_posz = rand.Float(0, 20);
	float cap_posx2 = rand.Float(0, 20);
	float cap_posy2 = rand.Float(0, 20);
	float cap_posz2 = rand.Float(0, 20);
	cap.l.a.Set(cap_posx, cap_posy, cap_posz);
	cap.l.b.Set(cap_posx2, cap_posy2, cap_posz2);
	cap.r = rand.Float(0, 10);

	if (sph.Intersects(cap))
	{
		LOG("INTERSECTED");
	}
	LOG("...........................");
}