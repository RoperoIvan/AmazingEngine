#include <stdio.h>
#include <conio.h>
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
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
	/*show_config_window = false;*/
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
	ManageUI(ret);
	CheckDrop();	

	return ret ? UPDATE_CONTINUE : UPDATE_STOP;
}

update_status GuiManager::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		debug_draw = !debug_draw;
	}

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

void GuiManager::ManageUI(bool open)
{
	//Menu top bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Console")) show_console_window = true;
			if (ImGui::MenuItem("Settings")) show_config_window = true;
			if (ImGui::MenuItem("Exit")) open = false;

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Hierarchy")) show_hierachy_window = true;
			if (ImGui::MenuItem("Textures")) show_textures_window = true;
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

	//Hierarchy window
	if (show_hierachy_window)
		HierarchyWindow();

	//Textures window
	if (show_textures_window)
		TexturesWindow();

}

// Settings of the engine tab
void GuiManager::ConfigurationWindow()
{
	if (show_config_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Settings", &show_config_window, window_flags))
		{
			ApplicationTab();
			HardwareTab();
			WindowTab();
			InputTab();
			RenderTab();
		}
		ImGui::End();
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
			ImGui::NewLine();
			if (ImGui::Button("DevIl 1.4.0"))
				App->RequestBrowser("http://openil.sourceforge.net/");
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
			if(ImGui::CollapsingHeader("Primitives"))
			{
				ImGui::ColorEdit4("Color", col);

				if(ImGui::Button("  Cube  "))
					CreatePrimitives(m, Primitives::CUBE, col, scale, translation, rad, axis);	
				else if (ImGui::Button(" Sphere "))
					CreatePrimitives(m, Primitives::SPHERE, col, scale, translation, rad, axis);
				else if (ImGui::Button("  Cone  "))
					CreatePrimitives(m, Primitives::CONE, col, scale, translation, rad, axis);
				else if (ImGui::Button("Cylinder"))
					CreatePrimitives(m, Primitives::CYILINDER, col, scale, translation, rad, axis);
				else if (ImGui::Button(" Plane  "))
					CreatePrimitives(m, Primitives::PLANE, col, scale, translation, rad, axis);
			}
		}
		ImGui::End();
	}
}


void GuiManager::GetLog(const char* log)
{
	console.AddLog(log);
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
	GameObject* game_object = new GameObject();
	Geometry* geo = dynamic_cast<Geometry*>(game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MESH));
	geo->CreatePrimitive(p_mesh,col[0], col[1], col[2], col[3]);
	App->scene->game_objects.push_back(game_object);
	//App->camera->GoAroundGeometry(&App->scene->game_objects);
	par_shapes_free_mesh(p_mesh);
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

void GuiManager::CheckDrop()
{
	const char* p_file = App->input->DragAndDropped();
	if (p_file != nullptr)
	{
		App->mesh->LoadFile(p_file);
		LOG("%s", p_file);
	}
}

void GuiManager::HierarchyWindow()
{
	if (show_hierachy_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Objects", &show_hierachy_window))
		{
			if (ImGui::CollapsingHeader("Geometry"))
			{
				static int selection_mask = (1 << 0); 
				static int node_clicked = 0;             

				for (uint i = 0; i < App->scene->game_objects.size(); ++i)
				{
					GameObject* game_object = App->scene->game_objects[i];

					if(game_object->parent == nullptr)
					{
						// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
						ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
						if (selection_mask & (1 << i))
							node_flags |= ImGuiTreeNodeFlags_Selected;

						bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, game_object->name.c_str());
						if (ImGui::IsItemClicked())
						{
							selection_mask = (1 << i);
							game_object->show_inspector_window = true;

							//al show inspector windows = false
							std::vector<GameObject*>::iterator iterator = App->scene->game_objects.begin();
							while (iterator != App->scene->game_objects.end())
							{
								if (*iterator != game_object)
									(*iterator)->show_inspector_window = false;
								++iterator;
							}

							if (game_object->children.size() != 0)
							{
								std::vector<GameObject*>::iterator child = game_object->children.begin();
								while (child != game_object->children.end())
								{
									if (*child != game_object)
										(*child)->show_inspector_window = false;
									++child;
								}
							}
						}
								
						if (game_object->show_inspector_window)
						{
							game_object->GetPropierties();
							App->scene->game_object_select = game_object;
						}
						if (node_open)
						{
							game_object->GetHierarcy();
							ImGui::TreePop();
						}
					}	
					
				}
			}
		}
		ImGui::End();
	}
}

void GuiManager::TexturesWindow()
{
	if (show_textures_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Textures", &show_textures_window))
		{
				for (uint i = 0; i < App->scene->textures.size(); ++i)
				{
					std::string node_name = "Texture " + std::to_string(i + 1);
					if (ImGui::TreeNodeEx(node_name.c_str()))
					{
						if (ImGui::Button("Delete"))
						{
							App->scene->DeleteTexture(App->scene->textures[i]);
							ImGui::TreePop();
							break;
						}
						ImVec2 size = { 200,200 };
						int id = App->scene->textures[i]->texture_id;
						ImGui::Image((ImTextureID)id, size);
						ImGui::TextColored(ImVec4(0, 0, 255, 255), "%i x %i", (int)size.x, (int)size.y);
						ImGui::TreePop();
					}
				}
		}
		ImGui::End();
	}
}

void GuiManager::ApplicationTab()
{
	if (ImGui::CollapsingHeader("Application"))
	{
		static char* str1 = "";
		ImGui::Text("App Name: ");
		ImGui::SameLine();
		if (ImGui::Button("Amazing Engine", ImVec2(357, 0)))
		{
			App->RequestBrowser("https://github.com/RoperoIvan/AmazingEngine");
		}
		ImGui::Text("Organitzation:");
		ImGui::SameLine();
		if (ImGui::Button("UPC CITM", ImVec2(357, 0)))
		{
			App->RequestBrowser("https://www.citm.upc.edu/");
		}
		int min = 0;
		int max = 144;

		if (ImGui::SliderInt("Max FPS", &maximum_fps, min, max))
		{
			App->MaxFrames(maximum_fps);
			
		}
		ImGui::TextWrapped("Limit Framerate: ");
		// FPS and MPF graphics logic
		int frames;
		float milisec;
		App->GetFrames(frames, milisec);

		if (fps_log.size() > 100)
		{
			fps_log.erase(fps_log.begin());
			ms_log.erase(ms_log.begin());
		}
		fps_log.push_back(frames);
		ms_log.push_back(milisec);

		LOG("%i", fps_log.size());

		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##Milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}
}

void GuiManager::HardwareTab()
{
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
		
		//Management of the VRam histogram
		VRamHardware();
	}
}

void GuiManager::WindowTab()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		bool check_vsync = App->renderer3D->vsync;
		if(ImGui::Checkbox("VSync", &check_vsync))
		{
			App->renderer3D->vsync = check_vsync;
			App->renderer3D->ChangeVSync();
		}
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
}

void GuiManager::InputTab()
{
	if (ImGui::CollapsingHeader("Input"))
	{
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "Mouse coordinates: x: %i y: %i", App->input->GetMouseX(), App->input->GetMouseY());
		char ch = getch();
		std::string key;
		key = (char)ch;
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "Last key pressed: %s", key.c_str());
		
	}
}

void GuiManager::RenderTab()
{
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
}

void GuiManager::VRamHardware()
{
	const GLubyte* v = glGetString(GL_VENDOR);
	char vendor[30];

	for (int i = 0; i<30; i++)
	{
		vendor[i] = v[i];
	}
	//Nvidia VRAM usage
	if (strcmp(vendor, "NVIDIA Corporation") == 0)
	{
		GLint total_mem_kb = 0;

		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_mem_kb);

		GLint cur_avail_mem_kb = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &cur_avail_mem_kb);

		//Calculate VRAM usage
		char vram[25];
		vram_log.push_back(((float)total_mem_kb) / 1000.f);
		if (vram_log.size() > 100)
		{
			for (int i = 0; vram_log.size() > i - 1; i++)
			{
				vram_log[i] = vram_log[i + 1];
			}
			vram_log.pop_back();
		}
		sprintf_s(vram, 25, "VRAM usage: %.3f Mb", ((float)total_mem_kb) / 1000.f);
		ImGui::PlotHistogram("##VRAM usage: ", &vram_log[0], vram_log.size(), 0, vram, 0.0f, 100.0f, ImVec2(310, 100));

		//Calculate VRAM available
		char vram_curr[50];
		curr_vram_log.push_back(((float)cur_avail_mem_kb) / 1000.f);
		if (curr_vram_log.size() > 100)
		{
			for (int i = 0; curr_vram_log.size() > i - 1; i++)
			{
				curr_vram_log[i] = curr_vram_log[i + 1];
			}
			curr_vram_log.pop_back();
		}
		sprintf_s(vram_curr, 50, "VRAM Available: %.3f Mb", ((float)cur_avail_mem_kb) / 1000.f);
		ImGui::PlotHistogram("##VRAM Available: ", &curr_vram_log[0], curr_vram_log.size(), 0, vram_curr, 0.0f, 100.0f, ImVec2(310, 100));

	}
	//AMD VRAM usage still in project
	//else if (strcmp(vendor, "ATI Technologies Inc.") == 0)
	//{
	//	GLint nCurAvailMemoryInKB = 0;
	//	glGetIntegerv(GL_ATI_meminfo,
	//	&nCurAvailMemoryInKB);
	//	char vram_curr[50];
	//	curr_vram_log.push_back(((float)nCurAvailMemoryInKB) / 1000.f);
	//	if (curr_vram_log.size() > 100)
	//	{
	//	for (int i = 0; curr_vram_log.size() > i - 1; i++)
	//	{
	//	curr_vram_log[i] = curr_vram_log[i + 1];
	//	}
	//	curr_vram_log.pop_back();
	//	}
	//	sprintf_s(vram_curr, 50, "VRAM Aviable: %.3f Mb", ((float)nCurAvailMemoryInKB) / 1000.f);
	//	ImGui::PlotHistogram("##VRAM Aviable: ", &curr_vram_log[0], curr_vram_log.size(), 0, vram_curr, 0.0f, 100.0f, ImVec2(310, 100));
	//	
	//}
	else (ImGui::TextColored(ImVec4(255,0,0,255),"VRam Usage only available for NVIDIA devices at the moment"));
}
