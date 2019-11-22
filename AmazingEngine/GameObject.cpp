#include "Application.h"
#include "GameObject.h"
#include "Image.h"
#include "Geometry.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Transform.h"
#include "Camera.h"
#include "ModuleMesh.h"


GameObject::GameObject(GameObject* parent): parent(parent)
{
	if (name.empty())
	{
		if (parent == nullptr)
			name = "GameObject " + std::to_string(App->scene->game_objects.size() + 1);
		else
			name = "GameObject " + std::to_string(App->scene->game_objects.size() + 1) + "." + std::to_string(parent->children.size() + 1);
	}
	bounding_box = new BoundingBox();
	bounding_box->aabb.SetNegativeInfinity();
	LCG rand;
	ID = rand.Int();
	is_static = true;

	if (is_static)
	{
		App->scene->octree->Insert(this);
	}
}

GameObject::~GameObject()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		(*it)->Disable();
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
			delete(*it);
			(*it) = nullptr;
			it = components.erase(it);
			break;
		}
		else if ((*it)->is_enable)
		{
			(*it)->Update();
		}
	}

	if (!children.empty())
	{
		for (std::vector<GameObject*>::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			if ((*iter)->to_delete)
			{
				if (*iter == App->scene->game_object_select)
					App->scene->game_object_select = nullptr;
				delete(*iter);
				(*iter) = nullptr;
				iter = children.erase(iter);
				break;
			}
			else if ((*iter)->is_enable)
			{
				(*iter)->Update();
			}
		}
	}

	if (show_bounding_box)
	{
		App->mesh->b_boxes.push(&bounding_box->aabb);
	}
}

void GameObject::Draw()
{
	Geometry* mesh = dynamic_cast<Geometry*>(GetComponentByType(COMPONENT_TYPE::COMPONENT_MESH));
	if (mesh != nullptr)
		mesh->DrawMesh();

	for (std::vector<GameObject*>::iterator iter = children.begin(); iter != children.end(); ++iter)
		(*iter)->Draw();
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
		//App->camera->GoAroundGeometry(&App->scene->game_objects);
		break;
	case COMPONENT_TYPE::COMPONENT_MATERIAL:
		component = new Image(this);
		components.push_back(component);
		break;
	case COMPONENT_TYPE::COMPONENT_CAMERA: // TODO: Need to change that so you can create the camera from here
		component = new Camera(this);
		//App->mesh->current_camera = (Camera*)component; //TODO: Move this to a function where you choose the main camera
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
			App->scene->game_object_select = game_object;
		}
	}
}

void GameObject::GetPropierties()
{
	if (ImGui::Begin("Inspector", &show_inspector_window))
	{
		if (ImGui::CollapsingHeader("Properties"))
		{
			//delete object
			if (ImGui::Button("Delete"))
				to_delete = true;

			//view object
			if (ImGui::Checkbox("Active", &is_enable))
				(&is_enable) ? true : false;

			if (ImGui::Checkbox("Static", &is_static))
			{
				(&is_static) ? true : false;
				if (is_static)
					App->scene->octree->Insert(this);
				else
					App->scene->octree->Remove(this);

			}
			//change name
	
			char a[100] = "";
			memcpy(a, name.c_str(),name.size());
			
			if (bool write = ImGui::InputText("", a, 100, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				name.assign(a);
			}
			if (ImGui::IsItemActive())
			{
				App->camera->write = true;
			}
			else
			{
				App->camera->write = false;
			}

			if (ImGui::Checkbox("show vertices normals", &show_vertices_normals))
			{
				(&show_vertices_normals) ? true : false;

				ShowNormalsVertices(show_vertices_normals);
			}

			ImGui::SameLine();

			if (ImGui::Checkbox("show faces normals", &show_face_normals))
			{
				(&show_face_normals) ? true : false;

				ShowNormalsFaces(show_face_normals);
			}
		}

		std::vector<Component*>::iterator it = components.begin();
		int id = 0;
		while ( it != components.end())
		{
			if ((*it)->type == COMPONENT_TYPE::COMPONENT_TRANSFORM)
			{
				dynamic_cast<Transform*>(*it)->LoadTransformation();
				break;
			}
			++it;
		}
		std::vector<Component*>::iterator it2 = components.begin();

		while (it != components.end())
		{
			if ((*it)->type == COMPONENT_TYPE::COMPONENT_CAMERA)
			{
				dynamic_cast<Camera*>(*it2)->LoadCameraOptions();
				break;
			}
			++it;
		}
		if (ImGui::CollapsingHeader("Information"))
		{
			uint num_vertices = 0;
			uint num_triangles = 0;

			ShowPropertiesObject(this, num_triangles, num_vertices);

			ImGui::Text("triangles: %u", num_triangles);
			ImGui::Text("vertices: %u", num_vertices);
			
			if (ImGui::Checkbox("Bounding Box",&show_bounding_box))
			{
				(&show_bounding_box) ? true : false;
			}
		}
		
		Component* tex = nullptr;
		std::vector<Component*>::iterator it3 = components.begin();
		while (it3 != components.end())
		{
			if ((*it3)->type == COMPONENT_TYPE::COMPONENT_MATERIAL)
			{
				tex = *it3;
				break;
			}
			++it3;
		}
		if(tex != nullptr)
			id = tex->GetTextureId();

		if (id != 0)
		{
			if (ImGui::CollapsingHeader("Material"))
			{
				Image* img = dynamic_cast<Image*>(tex);
				if (ImGui::Checkbox("Checker Texture", &img->show_checker_tex))
				{
					if (img->show_checker_tex)
					{
						img->SetTextureId(img->check_id);
					}
					else
					{
						img->SetTextureId(img->tmp_id);
					}
				}
				ImGui::Checkbox("show", &tex->show);
				ImGui::SameLine;
				ImVec2 size = { 200,200 };
				ImGui::Image((ImTextureID)id, size);
				ImGui::TextColored(ImVec4(255, 255, 0, 255), " Size: %i x %i", tex->tex_dimension[0], tex->tex_dimension[1]);
				ImGui::TextColored(ImVec4(255, 255, 0, 255), "Path: %s", tex->GetTexturePath().c_str());
			}
		}
		
		ImGui::End();
	}
}

void GameObject::ShowPropertiesObject(GameObject* object, uint& ntriangles, uint& nvertices)
{
	for (std::vector<Component*>::iterator it = object->components.begin(); it < object->components.end(); ++it)
	{
		if ((*it)->type == COMPONENT_TYPE::COMPONENT_MESH)
		{
			ntriangles += dynamic_cast<Geometry*>(*it)->num_indices / 3;
			nvertices += dynamic_cast<Geometry*>(*it)->num_vertices;
		}
	}
	if(!children.empty())
	{
		for (std::vector<GameObject*>::iterator iter = object->children.begin(); iter < object->children.end(); ++iter)
		{
			ShowPropertiesObject(*iter, ntriangles, nvertices);
		}
	}
}

void GameObject::ShowNormalsVertices(const bool& x)
{
	for (std::vector<GameObject*>::iterator iter = children.begin(); iter < children.end(); ++iter)
	{
		(*iter)->show_vertices_normals = x;
	}
}

void GameObject::ShowNormalsFaces(const bool& x)
{
	for (std::vector<GameObject*>::iterator iter = children.begin(); iter < children.end(); ++iter)
	{
		(*iter)->show_face_normals = x;
	}

}

void GameObject::LookForRayCollision(LineSegment ray_segment, std::vector<MouseHit>& hit)
{
	LookForMeshCollision(ray_segment, hit);
	for (int i = 0; i < children.size(); ++i)
	{
		children[i]->LookForRayCollision(ray_segment, hit);		
	}

}

void GameObject::LookForMeshCollision(LineSegment ray_segment, std::vector<MouseHit>& hit)
{
	Transform* transform = (Transform*)GetComponentByType(COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Geometry* mesh = (Geometry*)GetComponentByType(COMPONENT_TYPE::COMPONENT_MESH);

	if (mesh == nullptr)
		return;
	float* vertices = (float*)((Geometry*)mesh)->vertices;
	uint* indices = (uint*)((Geometry*)mesh)->indices;

	//Changin the ray into local space of the game objects
	LineSegment segment_localized = ray_segment;
	float4x4 inverted_m = transform->global_matrix.Transposed().Inverted();
	segment_localized = inverted_m*segment_localized;

	for (int j = 0; j < ((Geometry*)mesh)->num_indices;)
	{
		Triangle triangle;
		triangle.a.Set(&vertices[indices[j++] * 3]);
		triangle.b.Set(&vertices[indices[j++] * 3]);
		triangle.c.Set(&vertices[indices[j++] * 3]);

		float tmp_distance;
		if (segment_localized.Intersects(triangle, &tmp_distance, nullptr))
		{
			//Save all the hits
			MouseHit m_hit;
			m_hit.distance = tmp_distance;
			m_hit.object = this;
			hit.push_back(m_hit);
		}
	}
}

Component * GameObject::GetComponentByType(COMPONENT_TYPE type)
{
	for (std::vector<Component*>::iterator iter = components.begin(); iter < components.end(); ++iter)
	{
		if ((*iter)->type == type)
			return (*iter);
	}
	return false;
}

void GameObject::TransformBoundingBox(math::float4x4 matrix)
{
	// Generate global OBB
	if (bounding_box != nullptr)
	{
		bounding_box->obb.SetNegativeInfinity();
		bounding_box->obb = bounding_box->aabb.ToOBB();
		bounding_box->obb.Transform(matrix);
		// Generate global AABB
		bounding_box->aabb.SetNegativeInfinity();
		bounding_box->aabb.Enclose(bounding_box->obb);
	}
}

void GameObject::SaveMesh(FILE* file)
{
	std::fputs("GameObject:\n", file);

	if (parent != nullptr)
		std::fprintf(file, "parentID: %i;\n", parent->ID);
	else
		std::fprintf(file, "parentID: %i;\n", 0);

	std::fprintf(file, "ID: %i;\n", ID);

	std::fprintf(file, "name: %s;\n", name.c_str());
	for (std::vector<Component*>::iterator comp = components.begin(); comp != components.end(); ++comp)
	{
		if ((*comp)->type == COMPONENT_TYPE::COMPONENT_MESH)
			dynamic_cast<Geometry*>(*comp)->Save(file);
	}
	std::fprintf(file, "//\n", ID);
	if (children.size() > 0)
	{
		for (std::vector<GameObject*>::iterator iter = children.begin(); iter < children.end(); ++iter)
		{
			(*iter)->SaveMesh(file);
		}
	}
}

void GameObject::ImportMesh(char* &cursor, char* end_object)
{
	//assign ID
	std::stringstream convertor(App->file_system->DataValue(cursor, "ID:", 10));
	convertor >> ID;

	name.assign(App->file_system->DataValue(cursor, "name:", 20));
	name.pop_back();
	char* vertex = strstr(cursor, "vertices:");
	if (vertex < end_object)
	{
		Geometry* mesh = dynamic_cast<Geometry*>(CreateComponent(COMPONENT_TYPE::COMPONENT_MESH));
		mesh->transform = dynamic_cast<Transform*>(CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		mesh->texture = dynamic_cast<Image*>(CreateComponent(COMPONENT_TYPE::COMPONENT_MATERIAL));
		mesh->ImportNewMesh(cursor);
	}
}