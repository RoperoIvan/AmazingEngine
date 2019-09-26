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

private:

	bool show_demo_window = false;

	SDL_WindowFlags window_flags;
	SDL_GLContext gl_context;
	math::Sphere sph;
	math::Cylinder cyl;
	math::Capsule cap;
	math::Ray ray;
	math::AABB aabb;

};
#endif