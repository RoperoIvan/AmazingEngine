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

	//--indice elements
	float vertices[24] =
	{
		0.0, 0.0, 0.0, //0
		4.0, 0.0, 0.0, //1
		0.0, 40.0, 0.0, //2
	};
	GLubyte indices[36] =
	{ 0 ,1 ,2 };

	int num_of_vertex = 0;
	int my_indices = 0;
	int num_indices = 8;
	uint array_id = 0;
	AppConsole console;
	ImGuiTextBuffer text;

};
#endif
