#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <string>
#include <vector>
#include "Component.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	void Update();

	Component* CreateComponent(COMPONENT_TYPE type);

	virtual void GetHierarcy();

	void GetPropierties();

public:
	std::string name;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	bool show_inspector_window = true;
	bool to_delete = false;
};

#endif