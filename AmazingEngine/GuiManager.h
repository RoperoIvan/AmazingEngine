#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

class GuiManager: public Module
{
public:
	GuiManager(Application* app, bool start_enabled = true);
	~GuiManager();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	void CollisionsBetweenObjects();
	void AboutWindow(bool show_about_window);

private:

	bool show_demo_window = false;
	bool show_about_window = false;
	bool show_license_window = false;
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool full_desktop = false;

	SDL_WindowFlags window_flags;
	SDL_GLContext gl_context;
	math::Sphere sph;
	math::Cylinder cyl;
	math::Capsule cap;
	math::Ray ray;
	math::AABB aabb;

};
#endif
