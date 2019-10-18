#include "Transform.h"
#include "Globals.h"

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

void Transform::LoadTransformation(par_shapes_mesh* p_mesh,int trans[3], int scle[3], float rd, float axs[3])
{
	if (p_mesh != nullptr)
	{
		par_shapes_scale(p_mesh, scle[0], scle[1], scle[2]);
		par_shapes_translate(p_mesh, trans[0], trans[1], trans[2]);
		par_shapes_rotate(p_mesh, rd, axs);

		mesh = p_mesh;
		for (uint i = 0; i < 3; ++i)
		{
			translation[i] = -trans[i];
			scale[i] = -scle[i];
			axis[i] = axs[i];
		}
		rad = -rd;
	}
}

void Transform::UnLoadTransformation()
{
	par_shapes_scale(mesh, scale[0], scale[1], scale[2]);
	par_shapes_translate(mesh, translation[0], translation[1], translation[2]);
	par_shapes_rotate(mesh, rad, axis);

	to_delete = true;
}
