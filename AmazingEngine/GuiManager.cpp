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
#include "Primitive.h"

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


	ImGui::ShowDemoWindow();
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

	// Settings of the engine tab

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
			// Application options tab
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
				int frames;
				float milisec;
				App->GetFrames(frames, milisec);

				if (fps_log.size() > 100)
				{

					/*fps_log.pop_back();
					ms_log.pop_back();*/
					fps_log.erase(fps_log.begin());
				}

				fps_log.push_back(frames);
				ms_log.push_back(milisec);

				LOG("%i", fps_log.size());

				char title[25];
				sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
				ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
				/*sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
				ImGui::PlotHistogram("##Milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));*/

			}
			// Hardware specs tab
			if (ImGui::CollapsingHeader("Hardware"))
			{
				ImGui::TextWrapped("SDL version:");
				ImGui::SameLine();
				ImGui::TextColored({ 255, 255, 0, 255 }, "%i.%i.%i", App->system_specs.sdl_version.major, App->system_specs.sdl_version.minor, App->system_specs.sdl_version.patch);
				ImGui::TextWrapped("CPUs:", App->system_specs.cpus);
				ImGui::SameLine();
				ImGui::TextColored({ 255, 255, 0, 255 }, "%i", App->system_specs.cpus);
				ImGui::TextWrapped("System Ram:");
				ImGui::SameLine();
				ImGui::TextColored({ 255, 255, 0, 255 }, "%f", App->system_specs.system_ram);
				ImGui::TextWrapped("Caps:");
				if (App->system_specs.altivec)
				{
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, "AltiVec,");
				}					
				if (App->system_specs.rdtsc)
				{
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, "RDTSC,");
				}					
				if (App->system_specs.mmx)
				{
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, "MMX,");
				}					
				if (App->system_specs.sse)
				{
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, "SSE,");
				}					
				if (App->system_specs.sse2)
				{
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, "SSE2,");
				}				
				if (App->system_specs.sse3)
				{
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, "SSE3,");
				}					
				if (App->system_specs.sse41)
				{
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, "SSE41,");
				}					
				if (App->system_specs.sse42)
				{
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, "SSE42,");
				}					
				if (App->system_specs.three_d_now)
				{
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, "3DNow,");
				}					
				if (App->system_specs.avx)
				{
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, "AVX,");
				}					
				ImGui::Text("GPU vendor:");
				ImGui::SameLine();
				ImGui::TextColored({ 255, 255, 0, 255 }, "%s", glGetString(GL_VENDOR));
				ImGui::Text("GPU Model:");
				ImGui::SameLine();
				ImGui::TextColored({ 255, 255, 0, 255 }, "%s", glGetString(GL_RENDERER));
				ImGui::Text("GPU Drivers version:");
				ImGui::SameLine();
				ImGui::TextColored({ 255, 255, 0, 255 }, "%s", glGetString(GL_VERSION));
			}
			// Window tab
			if (ImGui::CollapsingHeader("Window"))
			{
				ImGui::Checkbox("Active", &App->renderer3D->vsync);
				ImGui::TextWrapped("Icon:  *default*");

				ImGui::SliderFloat("Brightness", &brightness, 0, 1);
				SDL_SetWindowBrightness(App->window->window, brightness);
				//windows size with combo
				const char* items[] = { "800x600", "1024x768", "1152x864", "1176x664", "1280x768", "1280x800", "1280x960", "1280x1024", "1360x768", "1440x900","1440x1280" };
				static int item_current = 7;
				if (ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items)))
				{
					switch (item_current)
					{
					case 0:
						SDL_SetWindowSize(App->window->window, 800, 600);
						break;
					case 1:
						SDL_SetWindowSize(App->window->window, 1152, 864);
						break;
					case 2:
						SDL_SetWindowSize(App->window->window, 1152, 864);
						break;
					case 3:
						SDL_SetWindowSize(App->window->window, 1176, 664);
						break;
					case 4:
						SDL_SetWindowSize(App->window->window, 1280, 768);
						break;
					case 5:
						SDL_SetWindowSize(App->window->window, 1280, 800);
						break;
					case 6:
						SDL_SetWindowSize(App->window->window, 1280, 960);
						break;
					case 7:
						SDL_SetWindowSize(App->window->window, 1280, 1024);
						break;
					case 8:
						SDL_SetWindowSize(App->window->window, 1360, 768);
						break;
					case 9:
						SDL_SetWindowSize(App->window->window, 1440, 900);
						break;
					case 10:
						SDL_SetWindowSize(App->window->window, 1440, 1280);
						break;
					default:
						break;
					}
				}
				//windows size with slide
				

				ImGui::TextWrapped("Refresh Rate: ");

				if (ImGui::Checkbox("Fullscreen", &fullscreen))
				{
					if (fullscreen)
					{
						SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
						fullscreen = true;
					}

					else
						fullscreen = false;
				}

				ImGui::SameLine();
				if (ImGui::Checkbox("Resizable", &resizable))
				{
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Restart to apply");
					if (resizable)
					{
						SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_RESIZABLE);
						resizable = true;
					}
					else
						resizable = false;
				}

				if (ImGui::Checkbox("Borderless", &borderless))
				{
					if (borderless)
					{
						SDL_SetWindowBordered(App->window->window, SDL_FALSE);
						borderless = true;
					}
					else
					{
						SDL_SetWindowBordered(App->window->window, SDL_TRUE);
						borderless = false;
					}
				}

				ImGui::SameLine();
				if (ImGui::Checkbox("Full Desktop", &full_desktop))
				{
					if (full_desktop)
					{
						SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
						full_desktop = true;
					}
					else
						full_desktop = false;
				}
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

	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	

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
	/*LCG rand;
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
	LOG("...........................");*/
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
