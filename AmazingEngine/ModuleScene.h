#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include "Module.h"
#include <vector>
#include "GameObject.h"
#include "Image.h"

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enable = true);
	~ModuleScene();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	void DrawPlane();
	update_status PostUpdate(float dt);
	bool CleanUp();
	GameObject* CreateGameObject(const char* object_name, GameObject* parent);

	void DeleteTexture(Image* tex);

	bool SaveScene(char* path);

public:
	
	uint nplane = 0;
	int* plane = nullptr;
	uint id = 0;
	std::vector<GameObject*> game_objects;
	std::vector<Image*> textures;
	GameObject* game_object_select = nullptr;
};
#endif
