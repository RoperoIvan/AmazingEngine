#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include "Module.h"
#include <vector>
#include "GameObject.h"

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enable = true);
	~ModuleScene();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:

	std::vector<GameObject*> game_objects;
	std::vector<int> textures;
};
#endif
