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

	void ManageUI(bool& open);

	void WindowPlay();

	//Windows in the main bar
	void AboutWindow();
	void ConfigurationWindow();
	void AppConsoleWindow();
	void PrimitivesWindow();
	void HierarchyWindow();
	void TexturesWindow();

	// Tabs from Configuration window
	void ApplicationTab();
	void HardwareTab();
	void VRamHardware();
	void RenderTab();
	void WindowTab();
	void InputTab();

	void GetLog(const char* log);
	void CreatePrimitives(par_shapes_mesh* p_mesh, Primitives prim, float color[4], int scale[3], int translation[3], float rad, const float* axis);
	void UIStyle();
	void CheckDrop();
private:

	bool show_config_window = false;
	bool show_about_window = false;
	bool show_console_window = true;
	bool show_primitives_window = false;
	bool show_hierachy_window = true;
	bool show_textures_window = false;

	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool full_desktop = false;

	std::vector<float> fps_log;
	std::vector<float> ms_log;
	std::vector<float> vram_log;
	std::vector<float> curr_vram_log;

	AppConsole console;
	Primitives prim = Primitives::UKNOWN;
	std::string test = "";
	ImGuiWindowFlags window_flags = 0;
	int maximum_fps = 0;

public:

	bool debug_draw = false;
	bool frustum_culling = false;
	bool active_octree = true;

};
#endif
