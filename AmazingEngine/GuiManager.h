#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include <vector>
#include "ConsoleManager.h"
#include "Geometry.h"


typedef unsigned char GLubyte;
typedef float GLfloat;

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
	void DrawGeometry();
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
	//------------------
	int num_of_vertex = 36;
	uint array_id = 0;
	uint plane_id = 1;
	AppConsole console;
	ImGuiTextBuffer text;
	//Indeces
	GLfloat vertices[24] = 
	{ 
		10,0,0,		12,0,0,
		12,2,0,		10,2,0,
		10,2,-2,	10,0,-2,
		12,0,-2,	12,2,-2 
	};

	GLubyte indices[36] = 
	  { 0,1,2, 2,3,0,   // 36 of indices
		0,3,4, 4,5,0,
		0,5,6, 6,1,0,
		1,6,7, 7,2,1,
		7,4,3, 3,2,7,
		4,7,6, 6,5,4 };

	uint my_id = 2;
	uint my_indices = 1;
	uint number_of_indices = 36;
	uint id_vertex = 0;
	uint id_index = 0;
};
#endif
