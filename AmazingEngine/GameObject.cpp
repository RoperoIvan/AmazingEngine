#include "GameObject.h"
#include "Image.h"
#include "Geometry.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Transform.h"

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
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->to_delete)
		{
			components.erase(it);
			break;
		}
		else if ((*it)->is_enable)
			(*it)->Update();
	}
}

Component* GameObject::CreateComponent(COMPONENT_TYPE type)
{
	Component* component = nullptr;
	switch (type)
	{
	case COMPONENT_TYPE::COMPONENT_TRANSFORM:
		component = new Transform(this);
		components.push_back(component);
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
				//delete object
				if (ImGui::MenuItem("Delete"))
					to_delete = true;

				ImGui::EndMenu();
			}
			//view oobject
			if (ImGui::Checkbox("Active", &is_enable))
				(&is_enable) ? true : false;

			//change name
			ImGui::SameLine();
			char a[100] = "";
			if (ImGui::InputText("", a, 100, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				name.assign(a);
			}
		}
		Component* mesh = nullptr;
		std::vector<Component*>::iterator it = components.begin();
		int id = 0;
		while ( it != components.end())
		{
			if ((*it)->type == COMPONENT_TYPE::COMPONENT_MESH)
			{
				mesh = *it;
				break;
			}
			++it;
		}
		if (mesh != nullptr)
			mesh->ShowProperties();

		Component* tex = nullptr;
		std::vector<Component*>::iterator it2 = components.begin();
		while (it2 != components.end())
		{
			if ((*it2)->type == COMPONENT_TYPE::COMPONENT_MATERIAL)
			{
				tex = *it2;
				break;
			}
			++it2;
		}
		if(tex != nullptr)
			id = tex->GetTextureId();

		if (id != 0)
		{
			ImVec2 size = {200,200};
			ImGui::Image((ImTextureID)id, size);
			ImGui::TextColored(ImVec4(0, 0, 255, 255), "%i x %i", (int)size.x, (int)size.y);
		}
		ImGui::End();
	}
}
