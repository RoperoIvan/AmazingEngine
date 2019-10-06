#include <stdio.h>
#include "Application.h"
#include "ModuleWindow.h"
#include "GuiManager.h"
#include "ModuleRenderer3D.h"
#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "ImGui/imgui.h"
#include"ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl.h"
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_opengl.h"
#include <fstream>
#include <iomanip>
#include "PhysFS/include/physfs.h"

#pragma comment (lib, "PhysFS/libx86/physfs.lib")

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

	show_config_window = false;
	return true;
}

bool GuiManager::Start()
{
	
	/*glGenBuffers(1, &id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->npoints * 3, m->points, GL_STATIC_DRAW);

	glGenBuffers(1, &id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T)*m->ntriangles * 3, m->triangles, GL_STATIC_DRAW);*/
	
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
			if (ImGui::MenuItem("Console")) show_console_window = true;
			if (ImGui::MenuItem("Settings")) show_config_window = true;
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
	//Configuration window
	if (show_config_window)
		ConfigurationWindow(show_config_window);
	
	//About window
	if (show_about_window)
		AboutWindow(show_about_window);

	//Console window
	if(show_console_window)
		ShowAppConsole(show_console_window);

	//PROVE
	const char* p_file = App->input->DragAndDropped();
	if (p_file != nullptr)
	{
		App->mesh->LoadFile(p_file);
		LOG("%s", p_file);
	}

	return ret ? UPDATE_CONTINUE : UPDATE_STOP;
}

update_status GuiManager::Update(float dt)
{

	return UPDATE_CONTINUE;
}

update_status GuiManager::PostUpdate(float dt)
{
	DrawGeometry();
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

bool GuiManager::Save(nlohmann::json & j) const
{
	return true;
}

bool GuiManager::Load(nlohmann::json & j)
{
	return true;
}

void GuiManager::ConfigurationWindow(bool show_conf_window)
{
	// Settings of the engine tab

	if (show_config_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Settings", &show_config_window), window_flags)
		{
			// Application options tab
			if (ImGui::CollapsingHeader("Application"))
			{
				static char* str1 = "Amazing Engine";
				ImGui::Text("App Name:     ");
				ImGui::SameLine(); ImGui::InputText(" ", str1, IM_ARRAYSIZE(str1));

				ImGui::Text("Organitzation:");
				ImGui::SameLine();
				if (ImGui::Button("UPC CITM", ImVec2(357, 0)))
				{
					App->RerquestBrowser("https://www.citm.upc.edu/");
				}
				uint min = 0;
				uint max = 144;

				ImGui::SliderScalar("Max FPS", ImGuiDataType_U32, &App->framerate_cap, &min, &max, "%d");

				// FPS and MPF graphics logic
				int frames;
				float milisec;
				App->GetFrames(frames, milisec);

				if (fps_log.size() > 100)
				{
					fps_log.erase(fps_log.begin());
					ms_log.erase(ms_log.begin());
				}
				//TODO: delete memory from vectors in the clean up
				fps_log.push_back(frames);
				ms_log.push_back(milisec);

				LOG("%i", fps_log.size());

				char title[25];
				sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
				ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
				sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
				ImGui::PlotHistogram("##Milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
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

				ImGui::SliderFloat("Brightness", &App->window->brightness, 0, 1);
				SDL_SetWindowBrightness(App->window->window, App->window->brightness);
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

				ImGui::TextWrapped("Refresh Rate: ");

				//Window rescale options
				if (ImGui::Checkbox("Fullscreen", &fullscreen))
				{
					if (fullscreen)
					{
						SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
						fullscreen = true;
					}

					else
					{
						SDL_SetWindowFullscreen(App->window->window, 0);
						fullscreen = false;
					}
						

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
					{
						SDL_SetWindowFullscreen(App->window->window, 0);
						resizable = false;
					}
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
					{
						SDL_SetWindowFullscreen(App->window->window, 0);
						full_desktop = false;
					}						
				}
				//------------------------------------------------------------------------------------------------------------
			}
			if (ImGui::CollapsingHeader("Render"))
			{

				if (ImGui::Checkbox("GL Depth", &App->renderer3D->gl_depth_on))
					(&App->renderer3D->gl_depth_on) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

				else if (ImGui::Checkbox("GL Cull Face", &App->renderer3D->gl_cull_face_on))
					(App->renderer3D->gl_cull_face_on) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

				else if (ImGui::Checkbox("GL Lighting", &App->renderer3D->gl_lighting_on))
					(App->renderer3D->gl_lighting_on) ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);

				else if (ImGui::Checkbox("GL Color Material", &App->renderer3D->gl_color_material_on))
					(App->renderer3D->gl_color_material_on) ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);

				else if (ImGui::Checkbox("GL Texture 2D", &App->renderer3D->gl_texture_2D_on))
					(App->renderer3D->gl_texture_2D_on) ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);

				else if (ImGui::Checkbox("GL Line Smooth", &App->renderer3D->gl_smooth_line_on))
					(App->renderer3D->gl_smooth_line_on) ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);

				else if (ImGui::Checkbox("Hard Poly", &App->renderer3D->gl_hard_on))
					(App->renderer3D->gl_hard_on) ? glShadeModel(GL_FLAT) : glShadeModel(GL_SMOOTH);

				else if (ImGui::Checkbox("Wireframe mode", &App->renderer3D->gl_wireframe_on))
				{
					if (App->renderer3D->gl_wireframe_on)
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
			ImGui::End();
		}

	}
}

void GuiManager::AboutWindow(bool show_about_win)
{
	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("About us", &show_about_window))
	{
		ImGui::Text("Amazing Engine version 0.1");
		ImGui::NewLine();
		ImGui::Text("This engine is being developed by ");
		ImGui::SameLine();
		if(ImGui::Button("Didac Llop"))
			App->RerquestBrowser("https://github.com/didaclis");
		ImGui::SameLine();
		ImGui::Text("and");
		ImGui::SameLine();
		if (ImGui::Button("Ivan Ropero"))
			App->RerquestBrowser("https://github.com/RoperoIvan");
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
			if (ImGui::Button("SDL 2.0.4"))
				App->RerquestBrowser("https://www.libsdl.org/");
			ImGui::NewLine();
			if (ImGui::Button("GLEW 2.1.0"))
				App->RerquestBrowser("http://glew.sourceforge.net/");
			ImGui::NewLine();
			if (ImGui::Button("ImGui 1.7.3"))
				App->RerquestBrowser("https://github.com/ocornut/imgui");
			ImGui::NewLine();
			if(ImGui::Button("MathGeoLib 1.5.0"))
				App->RerquestBrowser("https://github.com/juj/MathGeoLib");
			ImGui::NewLine();
			if(ImGui::Button("OpenGL 3.1.0"))
				App->RerquestBrowser("https://www.khronos.org/registry/OpenGL-Refpages/es3.0/");
		}
	}
	ImGui::End();
}

void GuiManager::ShowAppConsole(bool show_console)
{
	console.Draw("Amazing Engine", &show_console_window);
}

void GuiManager::GetLog(const char* log)
{
	console.AddLog(log);
}

void GuiManager::DrawGeometry()
{
	for (std::vector<Geometry*>::iterator it = App->mesh->geometry.begin(); it != App->mesh->geometry.end(); it++)
	{
		(*it)->Draw();
	}
}