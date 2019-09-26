#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	guiManager = new GuiManager(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
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
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;
	dt = (float)ms_timer.Read() / 1000.0f;
	if (dt > maxFrames)
		dt = maxFrames;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	avg_fps = float(frame_count) / startup_time.Read()/10000;
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
	

	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend() && ret == true; ++item)
	{
		ret = (*item)->CleanUp();
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}