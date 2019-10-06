#include "Application.h"
#include <shellapi.h>
#include <fstream>
#include <iomanip>

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	mesh = new ModuleMesh(this);
	guiManager = new GuiManager(this);
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(mesh);
	AddModule(guiManager);
	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); ++item)
	{
		if (*item != NULL)        
		{                      
			delete *item;
			*item = NULL;
		}
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	//Console is functioning;
	is_console = true;

	// Call Init() in all modules

	for(std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == true; ++item)
	{
		ret = (*item)->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == true; ++item)
	{
		ret = (*item)->Start();
	}
	
	//Hardware specs from the pc executing the program
	SDL_GetVersion(&system_specs.sdl_version);
	system_specs.cpus = SDL_GetCPUCount();
	system_specs.system_ram = SDL_GetSystemRAM() / 1000.0f;
	system_specs.cpu_chache = SDL_GetCPUCacheLineSize();
	system_specs.altivec = SDL_HasAltiVec();
	system_specs.rdtsc = SDL_HasRDTSC();
	system_specs.mmx = SDL_HasMMX();
	system_specs.sse = SDL_HasSSE();
	system_specs.sse2 = SDL_HasSSE2();
	system_specs.sse3 = SDL_HasSSE3();
	system_specs.sse41 = SDL_HasSSE41();
	system_specs.sse42 = SDL_HasSSE42();
	system_specs.three_d_now = SDL_Has3DNow();
	system_specs.avx = SDL_HasAVX();

	ms_timer.Start();
	

	Load();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	if (maxFrames > frame_count)
	{
		frame_count++;
	}
	last_sec_frame_count++;
	dt = 1.0f / framerate_cap;

	ms_timer.Start();

}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (frame_count == frame_count++)
	{
		frame_ms = ms_time.Read();
		ms_time.Start();
	}

	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}


	avg_fps = float(frame_count) / startup_time.Read()/1000;
	uint last_frame_ms = frame_time.Read();
	frames_on_last_update = prev_last_sec_frame_count;

	if (framerate_cap > 0 && last_frame_ms < framerate_cap)
	{
		SDL_Delay(framerate_cap - last_frame_ms);
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == UPDATE_CONTINUE; ++item)
	{
		ret = (*item)->PreUpdate(dt);
	}

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == UPDATE_CONTINUE; ++item)
	{
		ret = (*item)->Update(dt);
	}

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == UPDATE_CONTINUE; ++item)
	{
		ret = (*item)->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	
	is_console = false;

	Save();

	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend() && ret == true; ++item)
	{
		ret = (*item)->CleanUp();
	}
	return ret;
}

bool Application::Save()
{
	bool ret = true;
	using jsonf = nlohmann::json;
	jsonf jsonfile;

	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend() && ret == true; ++item)
	{
		ret = (*item)->Save(jsonfile);
	}
	std::fstream file("Save.json");
	file << jsonfile;
	return ret;
}

bool Application::Load()
{
	bool ret = true;

	nlohmann::json j;
	std::ifstream ifs("Save.json");
	if (!ifs.is_open())
	{
		LOG("Error to load file", SDL_GetError());
	}
	else
	{
		ifs >> j;

		for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend() && ret == true; ++item)
		{
			ret = (*item)->Load(j);
		}
	}
	return ret;
}

void Application::RerquestBrowser(const char *url)
{
	ShellExecuteA(GetActiveWindow(),"open",url,NULL,NULL, SW_SHOWNORMAL);
}

void Application::GetFrames(int & frames, float & miliseconds)
{
	frames = frames_on_last_update - 1;
	miliseconds = frame_ms;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}