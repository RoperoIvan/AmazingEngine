#include "GameObject.h"
#include "Image.h"
#include "Geometry.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it) != nullptr)
			delete (*it);
		(*it) = nullptr;
	}
	components.clear();

	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		if ((*it) != nullptr)
			delete (*it);
		(*it) = nullptr;
	}
	children.clear();
}

void GameObject::Update()
{
	for (uint i = 0; i < components.size(); ++i)
	{
		if (components[i]->is_enable)
			components[i]->Update();
	}
}

Component* GameObject::CreateComponent(COMPONENT_TYPE type)
{
	Component* component = nullptr;
	switch (type)
	{
	case COMPONENT_TYPE::COMPONENT_TRANSFORM:
		break;
	case COMPONENT_TYPE::COMPONENT_MESH:
		component = new Geometry(this);
		components.push_back(component);
		break;
	case COMPONENT_TYPE::COMPONENT_MATERIAL:
		component = new Image(this);
		components.push_back(component);
		break;
	case COMPONENT_TYPE::NO_COMPONENT:
		break;
	default:
		break;
	}
	return component;
}
