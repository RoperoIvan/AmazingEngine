#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "Globals.h"

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


private:

	bool show_demo_window = true;
	bool show_another_window = false;

	SDL_WindowFlags window_flags;
	SDL_GLContext gl_context;

};


#endif
