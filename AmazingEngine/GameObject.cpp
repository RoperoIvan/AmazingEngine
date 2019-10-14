#include "GameObject.h"

GameObject::~GameObject()
{
}

void GameObject::Update()
{
}

Component* GameObject::CreateComponent(COMPONENT_TYPE type)
{
	return nullptr;
}
