#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <string>
#include "Globals.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

class GameObject;

enum class COMPONENT_TYPE
{
	COMPONENT_TRANSFORM,
	COMPONENT_MESH,
	COMPONENT_MATERIAL,
	COMPONENT_CAMERA,
	COMPONENT_AUDIO,
	NO_COMPONENT
};

class Component
{
public:
	Component(GameObject* parent, COMPONENT_TYPE type);
	~Component();

	virtual void Enable();
	virtual void Update() = 0;
	virtual void Disable();

	//virtual void ShowProperties() = 0;
	virtual int GetTextureId() = 0;
	virtual std::string GetTexturePath() = 0;
	GameObject* GetParentObject()
	{
		return parent;
	}
public:
	bool show = true;
	bool to_delete = false;
	bool is_enable = true;
	uint ID = 0;
	int tex_dimension[2];
	COMPONENT_TYPE type = COMPONENT_TYPE::NO_COMPONENT;
	GameObject* parent = nullptr;
};

#endif


