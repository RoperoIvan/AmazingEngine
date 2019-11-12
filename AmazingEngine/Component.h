#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <string>

class GameObject;

enum class COMPONENT_TYPE
{
	COMPONENT_TRANSFORM,
	COMPONENT_MESH,
	COMPONENT_MATERIAL,
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

	virtual int GetTextureId() = 0;
	virtual std::string GetTexturePath() = 0;
public:
	bool show = true;
	bool to_delete = false;
	bool is_enable = true;
	int tex_dimension[2];
	COMPONENT_TYPE type = COMPONENT_TYPE::NO_COMPONENT;
	GameObject* parent = nullptr;
};

#endif


