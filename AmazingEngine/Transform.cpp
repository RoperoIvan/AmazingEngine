#include "Transform.h"
#include "Globals.h"
#include "GameObject.h"
#include "Geometry.h"
#include "par_shapes.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include <vector>
#include "ImGui/imgui.h"
#include "Assimp/include/scene.h"
Transform::Transform(GameObject* parent):Component(parent,COMPONENT_TYPE::COMPONENT_TRANSFORM)
{
}

Transform::~Transform()
{
	
}

void Transform::Enable()
{
}

void Transform::Update()
{
}

void Transform::Disable()
{
}

void Transform::Init(const int& x, const int& y, const int& z)
{
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

bool Transform::LoadTransformation()
{
	bool ret = false;
	//change name
	//scale
	if (ImGui::InputFloat3("scale", (float*)&scale, 1, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		
		ret = true;
	}
	//position
	if (ImGui::InputFloat3("position", (float*)&position,1,ImGuiInputTextFlags_EnterReturnsTrue))
	{		
		ret = true;
	}
	//rotation
	if (ImGui::InputFloat3("rotation", (float*)&euler_angles, 1, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		rot = math::Quat::FromEulerXYZ(math::DegToRad(euler_angles).x, math::DegToRad(euler_angles).y, math::DegToRad(euler_angles).z);
		ret = true;
	}

	if (ret)
	{
		rotation_matrix = math::float4x4::FromTRS(position, rot, scale);
		RotateObjects(parent);
	}	

	return ret;
}

void Transform::RotateObjects(GameObject* object_to_rotate)
{
	for (std::vector<Component*>::iterator component_iterator = object_to_rotate->components.begin(); component_iterator != object_to_rotate->components.end(); ++component_iterator)
	{
		if ((*component_iterator)->type == COMPONENT_TYPE::COMPONENT_TRANSFORM)
		{
			Transform* mesh = dynamic_cast<Transform*>(*component_iterator);
			mesh->global_matrix = mesh->global_matrix * rotation_matrix;
		}
	}
	if (object_to_rotate->children.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = object_to_rotate->children.begin(); it != object_to_rotate->children.end(); ++it)
		{
			RotateObjects(*it);
		}
	}
}

void Transform::UnLoadTransformation()
{
	//if parent have childs apply the transformation in all of them 
	if (parent->children.size() != 0)
	{
		for (std::vector<GameObject*>::iterator it = parent->children.begin(); it != parent->children.end(); ++it)
		{
			UnLoadTransformation();
		}
	}

	to_delete = true;
}

