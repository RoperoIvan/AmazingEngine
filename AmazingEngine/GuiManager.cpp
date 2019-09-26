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

	//example menu
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Settings")) show_demo_window = true;
			if (ImGui::MenuItem("Exit")) ret = false;

			ImGui::EndMenu();
		}	

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation")) 
				App->RerquestBrowser("https://github.com/RoperoIvan/AmazingEngine/wiki");

			if (ImGui::MenuItem("Last version")) 
				App->RerquestBrowser("https://github.com/RoperoIvan/AmazingEngine/releases");

			if (ImGui::MenuItem("Report bug")) 
				App->RerquestBrowser("https://github.com/RoperoIvan/AmazingEngine/issues");

			ImGui::EndMenu();

		}
		ImGui::EndMainMenuBar();
	}

	//ImGui::ShowDemoWindow();

	if (show_demo_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Settings", &show_demo_window),window_flags)
		{
			if (ImGui::CollapsingHeader("Configuration"))
			{
				if (ImGui::BeginMenu("Help"))
				{

					ImGui::EndMenu();

				}
			}

			if (ImGui::CollapsingHeader("Application"))
			{
				static char str0[128] = "Amazing Engine";
				ImGui::Text("App Name:     ");
				ImGui::SameLine(); ImGui::InputText(" ", str0, IM_ARRAYSIZE(str0));

				ImGui::Text("Organitzation:");
				ImGui::SameLine();
				if (ImGui::Button("UPC CITM", ImVec2(357, 0)))
				{
					App->RerquestBrowser("https://www.citm.upc.edu/");
				}
				if (ImGui::CollapsingHeader("FPS"))
				{
					uint min = 0;
					uint max = 144;
					ImGui::SliderScalar("Max FPS", ImGuiDataType_U32, &App->maxFrames, &min, &max, "%d");
				}
			}

			if (ImGui::Button("Close", ImVec2(550, 0)))
			{
				show_demo_window = false;
			}
			ImGui::End();
		}

	}
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

	float ray_pos_x = rand.Float(0, 20);
	float ray_pos_y = rand.Float(0, 20);
	float ray_pos_z = rand.Float(0, 20);
	ray.pos.Set(ray_pos_x, ray_pos_y, ray_pos_z);
	float ray_dirx = rand.Float(0, 20);
	float ray_diry = rand.Float(0, 20);
	float ray_dirz = rand.Float(0, 20);
	ray.dir.Set(ray_dirx, ray_diry, ray_dirz);
	ray.dir.Normalize();

	if (sph.Intersects(cap))
	{
		LOG("SPHERE AND CAPSULE ARE INTERSECTED");
	}
	if (sph.Intersects(ray))
	{
		LOG("SPHERE AND RAY ARE INTERSECTED");
	}
	if (cap.Intersects(ray))
	{
		LOG("RAY AND CAPSULE ARE INTERSECTED");
	}
	LOG("...........................");
}