#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include <vector>
#include "ConsoleManager.h"

typedef unsigned char GLubyte;

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

	bool Save(nlohmann::json &j) const override;
	bool Load(nlohmann::json &j) override;
	void AboutWindow(bool show_about_window);
	void ConfigurationWindow(bool show_config_window);
	void ShowAppConsole(bool show_console);
	void GetLog(const char* log);
private:

	bool show_config_window = false;
	bool show_about_window = false;
	bool show_console_window = false;
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool full_desktop = false;
	
	std::vector<float> fps_log;
	std::vector<float> ms_log;
	SDL_WindowFlags window_flags;
	SDL_GLContext gl_context;

	//Geometry
	float vertex[120];
	float plane[480];
	//--indice elements
	float e_vetrex[24] =
	{	10,0,0,		12,0,0,
		12,2,0,		10,2,0,
		10,2,-2,	10,0,-2,
		12,0,-2,	12,2,-2};
	GLubyte e_index[36] = {1,2,4, 2,3,4,
						   2,7,3, 7,8,3,
						   6,1,5, 1,4,5,
						   4,3,5, 3,8,5,
						   1,2,7, 1,6,7,
						   6,7,8, 6,8,5};

	int num_of_vertex = 0;
	int my_indices = 0;
	int num_indices = 8;
	uint array_id = 0;
	uint element_id = 2;
	uint plane_id = 1;
	AppConsole console;
	ImGuiTextBuffer text;

};
#endif
