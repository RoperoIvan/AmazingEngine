#include "Application.h"
#include "Transform.h"
#include "Globals.h"
#include "GameObject.h"
#include "ModuleScene.h"
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
	global_matrix.Decompose(position, rot, scale);
	euler_angles = math::RadToDeg(rot.ToEulerXYZ());
	if (transform_now)
	{
		RotateObjects(parent);
		transform_now = false;
	}

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
	//scale
	if (App->motor_state == ENGINE_STATE::PLAY)
	{
		ImGui::InputFloat3("scale", (float*)&scale, 1, ImGuiInputTextFlags_None);
		ImGui::InputFloat3("position", (float*)&position, 1, ImGuiInputTextFlags_None);
		ImGui::InputFloat3("rotation", (float*)&euler_angles, 1, ImGuiInputTextFlags_None);
	}
	else
	{
		if (ImGui::InputFloat3("scale", (float*)&scale, 1, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			ret = true;
		}
		//position
		if (ImGui::InputFloat3("position", (float*)&position, 1, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			ret = true;
		}
		//rotation
		if (ImGui::InputFloat3("rotation", (float*)&euler_angles, 1, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			rot = math::Quat::FromEulerXYZ(math::DegToRad(euler_angles).x, math::DegToRad(euler_angles).y, math::DegToRad(euler_angles).z);
			ret = true;
		}

		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::SCALE;

		ImGuiIO& io = ImGui::GetIO();

		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		float4x4 view_matrix = App->scene->current_camera->frustum.ViewMatrix();
		float4x4 proj_matrix = App->scene->current_camera->frustum.ProjectionMatrix();
		view_matrix.Transpose();
		proj_matrix.Transpose();
		float4x4 trs_matrix = global_matrix;
		float3* corners = new float3[8];
		parent->bounding_box->obb.GetCornerPoints(corners);
		ImGuizmo::Manipulate(view_matrix.ptr(), proj_matrix.ptr(), mCurrentGizmoOperation, mCurrentGizmoMode, trs_matrix.ptr(), (float*)corners, NULL);
		if (ImGuizmo::IsUsing())
		{
			trs_matrix.Transpose();
			float3 new_pos;
			float3 new_scale;
			Quat new_q;
			trs_matrix.Decompose(new_pos, new_q, new_scale);

			if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
			{
				position = new_pos;
			}
			if (mCurrentGizmoOperation == ImGuizmo::SCALE)
			{
				scale = new_scale;
			}

			if (mCurrentGizmoOperation == ImGuizmo::ROTATE)
			{
				rot = new_q.Conjugated();
				float3 euler = math::RadToDeg(rot.ToEulerXYZ());
				euler_angles = -euler;
			}
			ret = true;
		}

		if (ret)
		{
			rotation_matrix = math::float4x4::FromTRS(position, rot, scale);
			transform_now = true;

			App->scene->octree->Remove(parent);
			App->scene->octree->Insert(parent);
		}
	}
	return ret;
}

void Transform::RotateObjects(GameObject* object_to_rotate)
{
	parent->TransformBoundingBox(global_matrix.Inverted());
	for (std::vector<Component*>::iterator component_iterator = object_to_rotate->components.begin(); component_iterator != object_to_rotate->components.end(); ++component_iterator)
	{
		if ((*component_iterator)->type == COMPONENT_TYPE::COMPONENT_TRANSFORM)
		{
			Transform* mesh = dynamic_cast<Transform*>(*component_iterator);
			mesh->global_matrix = float4x4::identity * rotation_matrix;
		}
	}
	if (object_to_rotate->children.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = object_to_rotate->children.begin(); it != object_to_rotate->children.end(); ++it)
		{
			RotateObjects(*it);
		}
	}
	parent->TransformBoundingBox(global_matrix);

}

//void Transform::HandleGizmos()
//{
//}
