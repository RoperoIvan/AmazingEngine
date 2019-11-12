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
	float new_position[3] = { 0, 0 ,0 };
	float new_scale[3] = { 0, 0 ,0 };
	float new_rotation[3] = { 0, 0 ,0};
	//change name
	//scale
	if (ImGui::InputFloat3("scale", new_scale, 1, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		scale.x += new_scale[0];
		scale.y += new_scale[1];
		scale.z += new_scale[2];
		rotation_matrix = math::float4x4::FromTRS(position.zero, rot.identity, {new_scale[0],new_scale[1],new_scale[2]});
		ret = true;
	}
	//position
	if (ImGui::InputFloat3("transicion", new_position,1,ImGuiInputTextFlags_EnterReturnsTrue))
	{
		position.x += new_position[0];
		position.y += new_position[1];
		position.z += new_position[2];
		rotation_matrix = math::float4x4::FromTRS({ new_position[0],new_position[1],new_position[2] }, rot.identity, scale.one);
		ret = true;
	}
	//rotation
	if (ImGui::InputFloat3("rotation", new_rotation, 1, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		math::float3 to_rotate;
		to_rotate.x = new_rotation[0];
		to_rotate.y = new_rotation[1];
		to_rotate.z = new_rotation[2];
		rot = math::Quat::FromEulerXYZ(math::DegToRad(to_rotate).x, math::DegToRad(to_rotate).y, math::DegToRad(to_rotate).z);
		euler_angles += to_rotate;
		rotation_matrix = math::float4x4::FromTRS(position.zero, rot, scale.one);
		ret = true;
	}

	if (ret)
	{
		RotateObjects(parent);
	}
	//if parent have childs apply the transformation in all of them 
	

	return ret;
}

void Transform::RotateObjects(GameObject* object_to_rotate)
{
	for (std::vector<Component*>::iterator component_iterator = object_to_rotate->components.begin(); component_iterator != object_to_rotate->components.end(); ++component_iterator)
	{
		if ((*component_iterator)->type == COMPONENT_TYPE::COMPONENT_MESH)
		{
			Geometry* mesh = dynamic_cast<Geometry*>(*component_iterator);
			DoRotation(mesh, rotation_matrix);
			mesh->ActualitzateBuffer();
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

void Transform::DoRotation(Geometry* mesh, float4x4 r_matrix)
{
	for (uint i = 0; i < mesh->num_vertices * 3; i += 3)
	{
		float v1 = r_matrix[0][0] * (mesh->vertices[i] - mesh->vertices[0]) + r_matrix[1][0] * (mesh->vertices[i + 1] - mesh->vertices[1]) + r_matrix[2][0] * (mesh->vertices[i + 2] - mesh->vertices[2]);
		float v2 = r_matrix[0][1] * (mesh->vertices[i] - mesh->vertices[0]) + r_matrix[1][1] * (mesh->vertices[i + 1] - mesh->vertices[1]) + r_matrix[2][1] * (mesh->vertices[i + 2] - mesh->vertices[2]);
		float v3 = r_matrix[0][2] * (mesh->vertices[i] - mesh->vertices[0]) + r_matrix[1][2] * (mesh->vertices[i + 1] - mesh->vertices[1]) + r_matrix[2][2] * (mesh->vertices[i + 2] - mesh->vertices[2]);
		mesh->vertices[i] = v1;
		mesh->vertices[i + 1] = v2;
		mesh->vertices[i + 2] = v3;

		if (mesh->normals)
		{
			float n1 = r_matrix[0][0] * (mesh->vertices[i] - mesh->vertices[0]) + r_matrix[1][0] * (mesh->vertices[i + 1] - mesh->vertices[1]) + r_matrix[2][0] * (mesh->vertices[i + 2] - mesh->vertices[2]);
			float n2 = r_matrix[0][1] * (mesh->vertices[i] - mesh->vertices[0]) + r_matrix[1][1] * (mesh->vertices[i + 1] - mesh->vertices[1]) + r_matrix[2][1] * (mesh->vertices[i + 2] - mesh->vertices[2]);
			float n3 = r_matrix[0][2] * (mesh->vertices[i] - mesh->vertices[0]) + r_matrix[1][2] * (mesh->vertices[i + 1] - mesh->vertices[1]) + r_matrix[2][2] * (mesh->vertices[i + 2] - mesh->vertices[2]);
			mesh->normals[0] = n1;
			mesh->normals[1] = n2;
			mesh->normals[2] = n3;
		}
	}
}
