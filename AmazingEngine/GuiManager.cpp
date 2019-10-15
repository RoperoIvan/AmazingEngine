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
	UIStyle();
	ImGui_ImplOpenGL3_Init();
	show_config_window = false;
	return true;
}

bool GuiManager::Start()
{
	//App->mesh->LoadFile("../Assets/BakerHouse.fbx");

	return true;
}

update_status GuiManager::PreUpdate(float dt)
{
	bool ret = true;

	//Menu top bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Console")) show_console_window = true;
			if (ImGui::MenuItem("Settings")) show_config_window = true;
			if (ImGui::MenuItem("Exit")) ret = false;

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Inspector")) show_inspector_window = true;

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Primitives")) show_primitives_window = true;

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation"))
				App->RequestBrowser("https://github.com/RoperoIvan/AmazingEngine/wiki");

			if (ImGui::MenuItem("Last version"))
				App->RequestBrowser("https://github.com/RoperoIvan/AmazingEngine/releases");

			if (ImGui::MenuItem("Report bug"))
				App->RequestBrowser("https://github.com/RoperoIvan/AmazingEngine/issues");

			if (ImGui::MenuItem("About", NULL, show_about_window)) show_about_window = !show_about_window;

			ImGui::EndMenu();

		}
		ImGui::EndMainMenuBar();
	}
	//Configuration window
	if (show_config_window)
		ConfigurationWindow();

	//About window
	if (show_about_window)
		AboutWindow();

	//Console window
	if (show_console_window)
		AppConsoleWindow();

	//Primitives window
	if (show_primitives_window)
		PrimitivesWindow();

	//TODO: Move this somewhere where it has more sense to be written
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
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		debug_draw = !debug_draw;
	}
	ImGui::ShowDemoWindow();


	return UPDATE_CONTINUE;
}

update_status GuiManager::PostUpdate(float dt)
{
	DrawGeometry();

	return UPDATE_CONTINUE;
}

bool GuiManager::CleanUp()
{
	for (std::vector<Geometry*>::iterator it = geoms.begin(); it != geoms.end(); it++)
	{
		if ((*it) != nullptr)
			delete (*it);
		(*it) = nullptr;
	}
	fps_log.clear();
	ms_log.clear();

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

// Settings of the engine tab
void GuiManager::ConfigurationWindow()
{
	if (show_config_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Settings", &show_config_window))
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
					App->RequestBrowser("https://www.citm.upc.edu/");
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
//Window about info of the creators
void GuiManager::AboutWindow()
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
			App->RequestBrowser("https://github.com/didaclis");
		ImGui::SameLine();
		ImGui::Text("and");
		ImGui::SameLine();
		if (ImGui::Button("Ivan Ropero"))
			App->RequestBrowser("https://github.com/RoperoIvan");
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
				App->RequestBrowser("https://www.libsdl.org/");
			ImGui::NewLine();
			if (ImGui::Button("GLEW 2.1.0"))
				App->RequestBrowser("http://glew.sourceforge.net/");
			ImGui::NewLine();
			if (ImGui::Button("ImGui 1.7.3"))
				App->RequestBrowser("https://github.com/ocornut/imgui");
			ImGui::NewLine();
			if(ImGui::Button("MathGeoLib 1.5.0"))
				App->RequestBrowser("https://github.com/juj/MathGeoLib");
			ImGui::NewLine();
			if(ImGui::Button("OpenGL 3.1.0"))
				App->RequestBrowser("https://www.khronos.org/registry/OpenGL-Refpages/es3.0/");
		}
	}
	ImGui::End();
}

void GuiManager::AppConsoleWindow()
{
	console.Draw("Amazing Engine", &show_console_window);
}
//Window that allows to create primitives based in the specs that the user passes to the CreatePrimitives function
void GuiManager::PrimitivesWindow()
{
	if (show_primitives_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		par_shapes_mesh* m = nullptr;
		if (ImGui::Begin("Create Primitives", &show_primitives_window))
		{
			static float col[4] = { 0.4f,0.7f,0.0f,0.5f };
			static int scale[3] = { 1,1,1 };
			static int translation[3] = { 1,1,1 };
			static int rad = 0;
			static float axis[3] = {0,0,0};
			if(ImGui::CollapsingHeader("Cube"))
			{
				ImGui::SliderInt3("Size", scale, 1, 10);
				ImGui::SliderInt3("Translation", translation, 0, 100);
				ImGui::TextWrapped("Rotation");
				ImGui::Separator();
				ImGui::SliderInt("Radiant", &rad, 0, 360);
				const char* items[] = { "X", "Y", "Z"};
				static int item_current = 0;
				ImGui::Combo("Axis", &item_current, items, IM_ARRAYSIZE(items));
				switch (item_current)
				{
				case 0:
					axis[0] = 1;
					axis[1] = 0;
					axis[2] = 0;
					break;
				case 1:
					axis[0] = 0;
					axis[1] = 1;
					axis[2] = 0;
					break;
				case 2:
					axis[0] = 0;
					axis[1] = 0;
					axis[2] = 1;
					break;
				}
				DegToRad(rad);
				ImGui::ColorEdit4("Color", col);
				if(ImGui::Button("Create"))
					CreatePrimitives(m, Primitives::CUBE, col, scale, translation, rad, axis);				
			}
			if (ImGui::CollapsingHeader("Sphere"))
			{
				ImGui::SliderInt3("Size", scale, 1, 10);
				ImGui::SliderInt3("Translation", translation, 0, 100);
				ImGui::TextWrapped("Rotation");
				ImGui::Separator();
				ImGui::SliderInt("Radiant", &rad, 0, 360);
				const char* items[] = { "X", "Y", "Z" };
				static int item_current = 0;
				ImGui::Combo("Axis", &item_current, items, IM_ARRAYSIZE(items));
				switch (item_current)
				{
				case 0:
					axis[0] = 1;
					axis[1] = 0;
					axis[2] = 0;
					break;
				case 1:
					axis[0] = 0;
					axis[1] = 1;
					axis[2] = 0;
					break;
				case 2:
					axis[0] = 0;
					axis[1] = 0;
					axis[2] = 1;
					break;
				}
				DegToRad(rad);
				ImGui::ColorEdit4("Color", col);
				if (ImGui::Button("Create"))
					CreatePrimitives(m, Primitives::SPHERE, col, scale, translation, rad, axis);
			}
			if (ImGui::CollapsingHeader("Cone"))
			{
				ImGui::SliderInt3("Size", scale, 1, 10);
				ImGui::SliderInt3("Translation", translation, 0, 100);
				ImGui::TextWrapped("Rotation");
				ImGui::Separator();
				ImGui::SliderInt("Radiant", &rad, 0, 360);
				const char* items[] = { "X", "Y", "Z" };
				static int item_current = 0;
				ImGui::Combo("Axis", &item_current, items, IM_ARRAYSIZE(items));
				switch (item_current)
				{
				case 0:
					axis[0] = 1;
					axis[1] = 0;
					axis[2] = 0;
					break;
				case 1:
					axis[0] = 0;
					axis[1] = 1;
					axis[2] = 0;
					break;
				case 2:
					axis[0] = 0;
					axis[1] = 0;
					axis[2] = 1;
					break;
				}
				DegToRad(rad);
				ImGui::ColorEdit4("Color", col);
				if (ImGui::Button("Create"))
					CreatePrimitives(m, Primitives::CONE, col, scale, translation, rad, axis);
			}
			if (ImGui::CollapsingHeader("Cylinder"))
			{
				ImGui::SliderInt3("Size", scale, 1, 10);
				ImGui::SliderInt3("Translation", translation, 0, 100);
				ImGui::TextWrapped("Rotation");
				ImGui::Separator();
				ImGui::SliderInt("Radiant", &rad, 0, 360);
				const char* items[] = { "X", "Y", "Z" };
				static int item_current = 0;
				ImGui::Combo("Axis", &item_current, items, IM_ARRAYSIZE(items));
				switch (item_current)
				{
				case 0:
					axis[0] = 1;
					axis[1] = 0;
					axis[2] = 0;
					break;
				case 1:
					axis[0] = 0;
					axis[1] = 1;
					axis[2] = 0;
					break;
				case 2:
					axis[0] = 0;
					axis[1] = 0;
					axis[2] = 1;
					break;
				}
				DegToRad(rad);
				ImGui::ColorEdit4("Color", col);
				if (ImGui::Button("Create"))
					CreatePrimitives(m, Primitives::CYILINDER, col, scale, translation, rad, axis);
			}
			if (ImGui::CollapsingHeader("Plane"))
			{
				ImGui::SliderInt3("Size", scale, 1, 10);
				ImGui::SliderInt3("Translation", translation, 0, 100);
				ImGui::TextWrapped("Rotation");
				ImGui::Separator();
				ImGui::SliderInt("Radiant", &rad, 0, 360);
				const char* items[] = { "X", "Y", "Z" };
				static int item_current = 0;
				ImGui::Combo("Axis", &item_current, items, IM_ARRAYSIZE(items));
				switch (item_current)
				{
				case 0:
					axis[0] = 1;
					axis[1] = 0;
					axis[2] = 0;
					break;
				case 1:
					axis[0] = 0;
					axis[1] = 1;
					axis[2] = 0;
					break;
				case 2:
					axis[0] = 0;
					axis[1] = 0;
					axis[2] = 1;
					break;
				}
				DegToRad(rad);
				ImGui::ColorEdit4("Color", col);
				if (ImGui::Button("Create"))
					CreatePrimitives(m, Primitives::PLANE, col, scale, translation, rad, axis);
			}
			ImGui::End();
		}
	}
}


void GuiManager::GetLog(const char* log)
{
	console.AddLog(log);
}

void GuiManager::DrawGeometry()
{
	//for (std::vector<Geometry*>::iterator it = App->mesh->geometry.begin(); it != App->mesh->geometry.end(); it++)
	//{
	//	/*(*it)->Draw();
	//	if (debug_draw)
	//		(*it)->DebugDraw();*/
	//}
	for (std::vector<Geometry*>::iterator it = geoms.begin(); it != geoms.end(); it++)
	{
		(*it)->DrawPrimitives();
		if (debug_draw)
			(*it)->DebugDraw();
	}
}

void GuiManager::CreatePrimitives(par_shapes_mesh* p_mesh, Primitives prim, float col[4], int scale[3], int translation[3], float rad, const float* axis)
{
	switch (prim)
	{
	case Primitives::CUBE:
		p_mesh = par_shapes_create_cube();
		break;
	case Primitives::SPHERE:
		p_mesh = par_shapes_create_subdivided_sphere(5);
		break;
	case Primitives::CONE:
		p_mesh = par_shapes_create_cone(30, 3);
		break;
	case Primitives::CYILINDER:
		p_mesh = par_shapes_create_cylinder(30, 3);
		break;
	case Primitives::PLANE:
		p_mesh = par_shapes_create_plane(30, 3);
		break;
	default:
		LOG("Unknown primtive selected");
		break;
	}
	par_shapes_scale(p_mesh, scale[0], scale[1], scale[2]);
	par_shapes_translate(p_mesh, translation[0], translation[1], translation[2]);
	par_shapes_rotate(p_mesh, rad, axis);
	Geometry* geo = new Geometry(p_mesh->points, p_mesh->triangles, p_mesh->normals, p_mesh->npoints, p_mesh->ntriangles,col[0], col[1], col[2], col[3]);
	geoms.push_back(geo);
	App->camera->GoAroundGeometry(geo);
}

void GuiManager::UIStyle()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}
