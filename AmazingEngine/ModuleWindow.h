#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	bool Save(nlohmann::json &j) const override;
	bool Load(nlohmann::json &j) override;

	void SetTitle(const char* title);

	float2 GetWindowSettings();
public:
	//The window we'll be rendering to
	SDL_Window* window;
	float brightness = 1.0f;
	//The surface contained by the window
	SDL_Surface* screen_surface;
};

#endif // __ModuleWindow_H__