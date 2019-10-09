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
#include "par_shapes.h"


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
	void PrimitivesWindow();
	void GetLog(const char* log);
	void DrawGeometry();
	void CreatePrimitives(par_shapes_mesh* p_mesh, Primitives prim, float color[4]);
private:

	bool show_config_window = false;
	bool show_about_window = false;
	bool show_console_window = false;
	bool show_primitives_window = false;
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool full_desktop = false;
	bool debug_draw = false;
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
	Primitives prim = Primitives::UKNOWN;
	std::vector<Geometry*> geoms;
};
#endif
