#include "Component.h"
#include "GameObject.h"


Component::Component(GameObject* parent, COMPONENT_TYPE type): parent(parent), type(type)
{
	LCG rand;
	ID = rand.Int();
}

Component::~Component()
{
}

void Component::Enable()
{
	is_enable = true;
}

void Component::Disable()
{
	is_enable = false;
}

