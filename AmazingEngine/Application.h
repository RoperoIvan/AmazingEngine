#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "GuiManager.h"
#include "ModuleMesh.h"
#include "ModuleScene.h"
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
	ModuleMesh* mesh;
	ModuleScene* scene;
private:

	Timer	ms_timer;
	uint last_frame_ms;
	uint frame_ms;
	std::list<Module*> list_modules;
public:
	bool					is_console;
	Timer					ptimer;
	Timer					ms_time;
	uint					frame_count = 0u;
	Timer					startup_time;
	Timer					last_sec_frame_time;
	uint					last_sec_frame_count = 0u;
	uint					prev_last_sec_frame_count = 0u;
	uint16_t				framerate_cap = 0u;
	uint16_t				miliseconds_cap = 0u;
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

	bool Save();
	bool Load();

	void GetFrames(int& frames, float& miliseconds);
	void MaxFrames(int maximum);
	void RequestBrowser(const char*);
	HardwareSpecs system_specs;
private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;