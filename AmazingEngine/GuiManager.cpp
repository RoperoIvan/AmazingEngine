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

			if (ImGui::MenuItem("About", NULL, show_about_window)) show_about_window = !show_about_window;

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
			if (ImGui::CollapsingHeader("Hardware"))
			{
				ImGui::TextWrapped("SDL Version: %i.%i.%i", App->system_specs.sdl_version.major, App->system_specs.sdl_version.minor, App->system_specs.sdl_version.patch);
				ImGui::TextWrapped("CPUs: %i", App->system_specs.cpus);
				ImGui::TextWrapped("System Ram: %f", App->system_specs.system_ram);
				if (App->system_specs.altivec)
					ImGui::TextWrapped("AltiVec");
				if (App->system_specs.rdtsc)
					ImGui::TextWrapped("RDTSC");
				if (App->system_specs.mmx)
					ImGui::TextWrapped("MMX");
				if (App->system_specs.sse)
					ImGui::TextWrapped("SSE");
				if (App->system_specs.sse2)
					ImGui::TextWrapped("SSE2");
				if (App->system_specs.sse3)
					ImGui::TextWrapped("SSE3");
				if (App->system_specs.sse41)
					ImGui::TextWrapped("SSE41");
				if (App->system_specs.sse42)
					ImGui::TextWrapped("SSE42");
				if (App->system_specs.three_d_now)
					ImGui::TextWrapped("3DNow");
				if (App->system_specs.avx)
					ImGui::TextWrapped("AVX");
				ImGui::Text("GPU vendor: %s", glGetString(GL_VENDOR));
				ImGui::Text("GPU Model: %s", glGetString(GL_RENDERER));
				ImGui::Text("GPU Drivers version: %s", glGetString(GL_VERSION));
			}
			if (ImGui::Button("Close", ImVec2(550, 0)))
			{
				show_demo_window = false;
			}
			ImGui::End();
		}

	}


	if (show_about_window)
	{
		AboutWindow(show_about_window);
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

void GuiManager::AboutWindow(bool show_about_win)
{
	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("About us", &show_about_win))
	{
		ImGui::Text("Amazing Engine version 0.1");
		ImGui::NewLine();
		ImGui::Text("This engine is being developed by Didac Llop and Ivan Ropero.");
		ImGui::NewLine();
		if (ImGui::CollapsingHeader("License"))
		{
			ImGui::Text("Copyright (c) 2019 Didac Llop Serna & Ivan Ropero Garcia");
			ImGui::NewLine();
			ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy\nof this software and associated documentation files(the 'Software'), to deal\nin the Software without restriction, including without limitation the rights\nto use, copy, modify, merge, publish, distribute, sublicense, and/or sell\ncopies of the Software, and to permit persons to whom the Software is\nfurnished to do so, subject to the following conditions :");
			ImGui::NewLine();
			ImGui::Text("The above copyright notice and this permission notice shall be included in all\ncopies or substantial portions of the Software.");
			ImGui::NewLine();
			ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\nSOFTWARE.");
			ImGui::NewLine();
		}
		if (ImGui::CollapsingHeader("Libraries"))
		{
			ImGui::Text("SDL 2.0.4");
			ImGui::NewLine();
			ImGui::Text("GLEW 2.1.0");
			ImGui::NewLine();
			ImGui::Text("ImGui 1.7.3");
			ImGui::NewLine();
			ImGui::Text("MathGeoLib 1.5.0");
			ImGui::NewLine();
			ImGui::Text("OpenGL 3.1.0");
			ImGui::NewLine();
		}
	}
	ImGui::End();
}