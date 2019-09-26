#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "GuiManager.h"
#include "Timer.h"

#include <list>
#include <vector>

struct HardwareSpecs
{
public:

	SDL_version sdl_version;
	int cpus = 0;
	float system_ram = 0;
	int cpu_chache = 0;
	bool rdtsc;
	bool mmx;
	bool sse;
	bool sse2;
	bool sse3;
	bool sse41;
	bool sse42;
	bool three_d_now;
	bool avx;
	bool avx2;
	bool altivec;

};

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	GuiManager* guiManager;

private:

	Timer	ms_timer;
	
	std::list<Module*> list_modules;

public:

	Timer					ptimer;
	uint					frame_count = 0u;
	Timer					startup_time;
	Timer					frame_time;
	Timer					last_sec_frame_time;
	uint					last_sec_frame_count = 0u;
	uint					prev_last_sec_frame_count = 0u;
	uint16_t				framerate_cap = 0u;
	float					avg_fps = 0.0f;
	uint					frames_on_last_update = 0u;
	float					dt;
	uint					maxFrames = 60u;
	
	std::list<uint>		lastFrames[100];
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	void GetFrames(int& frames, float& miliseconds);

	void RerquestBrowser(const char*);
	HardwareSpecs system_specs;
private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};