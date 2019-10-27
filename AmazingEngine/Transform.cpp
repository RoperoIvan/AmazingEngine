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

bool Transform::LoadTransformation(Geometry* mesh)
{
	bool ret = false;

	float new_position[3] = { position[0], position[1] ,position[2] };
	//change name
	//scale
	if(ImGui::InputFloat3("scale", scale, 1, ImGuiInputTextFlags_EnterReturnsTrue))
		ret = true;
	if (ImGui::InputFloat3("position", new_position,1,ImGuiInputTextFlags_EnterReturnsTrue))
	{
		float translation_x = new_position[0] - position[0];
		float translation_y = new_position[1] - position[1];
		float translation_z = new_position[2] - position[2];
		ChangePosition(mesh, translation_x, translation_y, translation_z);
		for (uint i = 0; i < 3; ++i)
		{
				position[i] = new_position[i];
		}
		ret = true;
	}
	ImGui::TextWrapped("Rotation");
	ImGui::Separator();
	if (ImGui::SliderInt("Radiant", &rad, 0, 360)) 
		ret = true;

	static int item_current = 0;
	const char* items[] = { "X", "Y", "Z" };
	ImGui::Combo("Axis", &item_current, items, IM_ARRAYSIZE(items));
	switch (item_current)
	{
	case 0:
		axis[0] = 1;
		axis[1] = 0;
		axis[2] = 0;
		break;
	case 1:
		axis[0] = 0;
		axis[1] = 1;
		axis[2] = 0;
		break;
	case 2:
		axis[0] = 0;
		axis[1] = 0;
		axis[2] = 1;
		break;
	}

	//if parent have childs apply the transformation in all of them 
	if (mesh != nullptr)
	{
		for (std::vector<GameObject*>::iterator it = parent->children.begin(); it != parent->children.end(); ++it)
		{
			Transform* comp = dynamic_cast<Transform*>((*it)->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
			comp->LoadTransformation(mesh);
		}
	}		

	return ret;
}

void Transform::UnLoadTransformation()
{
	//ChangeScale(mesh, scale[0], scale[1], scale[3]);
	ChangePosition(mesh, position[0], position[1], position[2]);
	//par_shapes_rotate(mesh, rad, axis);

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

void Transform::ChangeScale(Geometry* mesh, float x, float y, float z)
{
	for (uint i = 0; i < mesh->num_vertices; i += 3) {
		mesh->vertices[i] *= x;
		mesh->vertices[i + 1] *= y;
		mesh->vertices[i + 2] *= z;
	}
}

void Transform::ChangePosition(Geometry* mesh, float x, float y, float z)
{
	for (uint i = 0; i < mesh->num_vertices*3; i += 3) 
	{
		mesh->vertices[i] += x;
		mesh->vertices[i + 1] += y;
		mesh->vertices[i + 2] += z;

		if (mesh->normals != nullptr)
		{
			mesh->normals[i] += x;
			mesh->normals[i + 1] += y;
			mesh->normals[i + 2] += z;
		}
		if (mesh->face_normals != nullptr)
		{
			mesh->face_normals[i] += x;
			mesh->face_normals[i + 1] += y;
			mesh->face_normals[i + 2] += z;
		}
	}
}

void Transform::Rotate(Geometry* mesh, float rd, float axs[3])
{
	//calculate rotation matrix

	float rd_cos = cosf(rd);
	float rd_sin = sinf(rd);

	float r_matrix[3][3] =
	{
		(axs[0] * axs[0]) * (1-rd_cos) + rd_cos,(axs[0] * axs[1] * (1 - rd_cos)) + (axs[2] * rd_sin), (axs[2] * axs[0] * (1 - rd_cos)) - (axs[1] * rd_sin),
		(axs[0] * axs[1] * (1 - rd_cos)) - (axs[2] * rd_sin),(((axs[1] * axs[1]) * (1 - rd_cos)) + rd_cos), (axs[1] * axs[2] * (1 - rd_cos)) + (axs[0] * rd_sin),
		((axs[2] * axs[0] * (1 - rd_cos)) + (axs[1] * rd_sin)),((axs[1] * axs[2] * (1 - rd_cos)) - (axs[0] * rd_sin)), (((axs[2] * axs[2]) * (1 - rd_cos)) + rd_cos),
	};
	
	//do rotation
	DoRotation(mesh, r_matrix);

	//save rotation
	for (uint i = 0; i < 3; ++i)
	{
		for (uint j = 0; j < 3; ++j)
		{
			for (uint k = 0; k < 3; ++k)
			{
				for (uint l = 0; l < 3; ++l)
				{
					R[i][j] += R[k][l] * r_matrix[l][k];
				}
			}
		}
	}
}

void Transform::DoRotation(Geometry* mesh, float  r_matrix[3][3])
{
	for (uint i = 0; i < mesh->num_vertices * 3; i += 3)
	{

		float v1 = r_matrix[0][0] * mesh->vertices[i] + r_matrix[1][0] * mesh->vertices[i + 1] + r_matrix[2][0] * mesh->vertices[i + 2];
		float v2 = r_matrix[0][1] * mesh->vertices[i] + r_matrix[1][1] * mesh->vertices[i + 1] + r_matrix[2][1] * mesh->vertices[i + 2];
		float v3 = r_matrix[0][2] * mesh->vertices[i] + r_matrix[1][2] * mesh->vertices[i + 1] + r_matrix[2][2] * mesh->vertices[i + 2];
		mesh->vertices[i] = v1;
		mesh->vertices[i + 1] = v2;
		mesh->vertices[i + 2] = v3;

		if (mesh->normals)
		{
			float n1 = r_matrix[0][0] * mesh->normals[i] + r_matrix[1][0] * mesh->normals[i + 1] + r_matrix[2][0] * mesh->normals[i + 2];
			float n2 = r_matrix[0][1] * mesh->normals[i] + r_matrix[1][1] * mesh->normals[i + 1] + r_matrix[2][1] * mesh->normals[i + 2];
			float n3 = r_matrix[0][2] * mesh->normals[i] + r_matrix[1][2] * mesh->normals[i + 1] + r_matrix[2][2] * mesh->normals[i + 2];
			mesh->normals[0] = n1;
			mesh->normals[1] = n2;
			mesh->normals[2] = n3;
		}


	}
}
