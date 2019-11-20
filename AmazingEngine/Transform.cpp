#include "Application.h"
#include "Transform.h"
#include "Globals.h"
#include "GameObject.h"
#include "Geometry.h"
#include "par_shapes.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include <vector>
#include "ImGui/imgui.h"
#include "ImGui/ImGuizmo.h"
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

void Transform::Init(const float& x, const float& y, const float& z)
{
	position.x = x;
	position.y = y;
	position.z = z;
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

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
		mCurrentGizmoOperation = ImGuizmo::SCALE;


	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	float4x4 view_matrix = App->camera->my_camera->frustum.ViewMatrix();
	float4x4 proj_matrix = App->camera->my_camera->frustum.ProjectionMatrix();
	view_matrix.Transpose();
	proj_matrix.Transpose();
	float4x4 trs_matrix = global_matrix;
	ImGuizmo::Manipulate(view_matrix.ptr(), proj_matrix.ptr(), mCurrentGizmoOperation, mCurrentGizmoMode, trs_matrix.ptr(), NULL, NULL);

	if (ImGuizmo::IsUsing())
	{
		trs_matrix.Transpose();
		float3 new_pos;
		float3 new_scale;
		Quat new_q;
		trs_matrix.Decompose(new_pos, new_q, new_scale);

		rotation_matrix = math::float4x4::FromTRS(new_pos, rot.identity, scale.one);

		/*rotation_matrix = math::float4x4::FromTRS(new_pos - current_pos, rot.identity, scale.one);

		rotation_matrix = math::float4x4::FromTRS(new_pos - current_pos, rot.identity, scale.one);
		*/
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

//void Transform::HandleGizmos()
//{
//}
