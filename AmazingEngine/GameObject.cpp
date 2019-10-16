#include "GameObject.h"
#include "Image.h"
#include "Geometry.h"
#include "Application.h"
#include "ModuleScene.h"


GameObject::GameObject()
{
	name = "GameObject " + std::to_string(App->scene->game_objects.size());
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
		App->camera->GoAroundGeometry();
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

void GameObject::GetHierarcy()
{
	if (show_inspector_window)
		GetPropierties();
	for (uint i = 0; i < children.size(); ++i)
	{
		GameObject* game_object = children[i];

		if (game_object->children.size() != 0)
		{
			if (ImGui::TreeNodeEx(game_object->name.c_str()))
			{
				game_object->GetHierarcy();
				ImGui::TreePop();
			}
		}
		else
		{
			if (ImGui::TreeNodeEx(game_object->name.c_str()))
			{
				ImGui::TreePop();
			}
		}
	}
}

void GameObject::GetPropierties()
{
	if (ImGui::Begin("Inspector", &show_inspector_window))
	{
		if (ImGui::CollapsingHeader("Properties"))
		{
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Quit"))
					to_delete = true;

				ImGui::EndMenu();
			}


		}
		ImGui::End();
	}
}
