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
	math::float3 current_pos = position;
	math::float3 current_angles = euler_angles;
	//scale
	if (ImGui::InputFloat3("scale", (float*)&scale, 1, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		
		rotation_matrix = math::float4x4::FromTRS(position.zero, rot.identity, scale);
		ret = true;
		
	}
	//position
	if (ImGui::InputFloat3("position", (float*)&position,1,ImGuiInputTextFlags_EnterReturnsTrue))
	{		
		rotation_matrix = math::float4x4::FromTRS(position - current_pos, rot.identity, scale.one);
		ret = true;
	}
	//rotation
	if (ImGui::InputFloat3("rotation", (float*)&euler_angles, 1, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		rot = math::Quat::FromEulerXYZ(math::DegToRad(euler_angles - current_angles).x, math::DegToRad(euler_angles - current_angles).y, math::DegToRad(euler_angles - current_angles).z);
		rotation_matrix = math::float4x4::FromTRS(position.zero, rot, scale.one);
		ret = true;
	}

	if (ret)
	{
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
