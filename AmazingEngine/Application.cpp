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
	file_system = new FileSystem(this);
	mesh = new ModuleMesh(this);
	guiManager = new GuiManager(this);
	scene = new ModuleScene(this);
	audio = new ModuleAudio(this);
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(file_system);
	AddModule(mesh);
	AddModule(scene);
	AddModule(guiManager);
	AddModule(audio);
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

	game_time.Stop();
	// Call Init() in all modules

	for(std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == true; ++item)
	{
		ret = (*item)->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("-------------- Application Start --------------");

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == true; ++item)
	{
		ret = (*item)->Start();
	}
	
	MaxFrames(0);

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

	if (game_time.GetState() == TIMER_STATE::PAUSE)
		dtGame = 0;
	else
		dtGame = 1.0f / framerate_cap_game;

	if (do_iteration)
	{
		dtGame = 1.0f / framerate_cap_game;
		do_iteration = false;
		game_time.SetState(TIMER_STATE::PAUSE);
	}
	
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

	uint last_frame_ms = ms_timer.Read();
	frames_on_last_update = prev_last_sec_frame_count;

	if (framerate_cap > 0 && last_frame_ms < miliseconds_cap)
	{
		SDL_Delay(miliseconds_cap - last_frame_ms);
	}
	last_frame_ms = ms_timer.Read();
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
		if (engine_state == ENGINE_STATE::EXECUTE)
			ret = (*item)->GameUpdate(dtGame);
		else
			ret = (*item)->Update(dt);
	}

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == UPDATE_CONTINUE; ++item)
	{
		ret = (*item)->PostUpdate(dt);
	}

	if (input->GetKey(SDL_SCANCODE_F7))
		engine_state = ENGINE_STATE::EXECUTE;

	if (input->GetKey(SDL_SCANCODE_F8))
		engine_state = ENGINE_STATE::EDIT;

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
		LOG("Error to load file", SDL_GetError()); //?????
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

void Application::RequestBrowser(const char *url)
{
	ShellExecuteA(GetActiveWindow(),"open",url,NULL,NULL, SW_SHOWNORMAL);
}

void Application::GetFrames(int & frames, float & miliseconds)
{
	frames = frames_on_last_update - 1;
	miliseconds = frame_ms;
}

void Application::MaxFrames(int maximum)
{
	framerate_cap = maximum;
	miliseconds_cap = (framerate_cap > 0) ? 1000 / maximum : 0;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}