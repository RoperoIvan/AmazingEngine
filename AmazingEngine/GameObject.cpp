#include "Application.h"
#include "GameObject.h"
#include "Image.h"
#include "Geometry.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Transform.h"

GameObject::GameObject(GameObject* parent): parent(parent)
{
	if(parent == nullptr)
		name = "GameObject " + std::to_string(App->scene->game_objects.size() + 1);
	else
		name = "GameObject " + std::to_string(App->scene->game_objects.size() + 1) + "." + std::to_string(parent->children.size() + 1);
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
	if (children.empty())
	{
		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		{
			if ((*it)->to_delete)
			{
				components.erase(it);
				break;
			}
			else if ((*it)->is_enable)
			{
				(*it)->Update();
			}
		}
	}
	else
	{
		for (std::vector<GameObject*>::iterator iter = children.begin(); iter != children.end(); ++iter)
			(*iter)->Update();
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
		App->camera->GoAroundGeometry(&App->scene->game_objects);
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
	static int selection_mask = (1 << 0);
	static int node_clicked = 0;

	for (uint i = 0; i < children.size(); ++i)
	{
		GameObject* game_object = children[i];

		//start to show inspector
			// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (selection_mask & (1 << i))
			node_flags |= ImGuiTreeNodeFlags_Selected;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, game_object->name.c_str());
		if (ImGui::IsItemClicked())
		{
			selection_mask = (1 << i);
			//al show inspector windows = false
			std::vector<GameObject*>::iterator iterator = children.begin();
			while (iterator != children.end())
			{
				if (*iterator != game_object)
					(*iterator)->show_inspector_window = false;
				++iterator;
			}
			//parent inspector = false
			if(show_inspector_window)
				show_inspector_window = false;
			
			//show inspector
			game_object->show_inspector_window = true;
		}
		//finish show inspector

		
		if (node_open)
		{
			if (game_object->children.size() != 0)
			{
				game_object->GetHierarcy();
			}
			ImGui::TreePop();
		}
		
		if (game_object->show_inspector_window)
		{
			game_object->GetPropierties();
		}
	}
}

void GameObject::GetPropierties()
{
	if (ImGui::Begin("Inspector", &show_inspector_window))
	{
		if (ImGui::CollapsingHeader("Properties"))
		{
			if (ImGui::CollapsingHeader("Options"))
			{
				//delete object
				if (ImGui::Button("Delete"))
					to_delete = true;
			}
			//view oobject
			if (ImGui::Checkbox("Active", &is_enable))
				(&is_enable) ? true : false;

			//change name
			ImGui::SameLine();
			char a[100] = "";
			memcpy(a, name.c_str(),name.size());
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
				mesh->ShowProperties();
				break;
			}
			++it;
		}

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
			ImGui::TextColored(ImVec4(255, 255, 0, 255), " Size: %i x %i", tex->tex_dimension[0], tex->tex_dimension[1]);
			ImGui::TextColored(ImVec4(255, 255, 0, 255), "Path: %s", tex->GetTexturePath().c_str());
		}
		ImGui::End();
	}
}
