#ifndef _COMPONENT_H_
#define _COMPONENT_H_

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

	virtual void Enable() = 0;
	virtual void Update() = 0;
	virtual void Disable() = 0;

public:

	bool is_enable = true;
	COMPONENT_TYPE type = COMPONENT_TYPE::NO_COMPONENT;
	GameObject* parent = nullptr;
};

#endif
