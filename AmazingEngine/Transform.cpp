#include "Transform.h"
#include "Globals.h"
#include "GameObject.h"
#include "Geometry.h"
#include "par_shapes.h"

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

void Transform::LoadTransformation(Geometry* mesh,int trans[3], int scle[3], float rd, float axs[3])
{
	if (mesh != nullptr)
	{
		ChangeScale(mesh, scle[0], scle[1], scle[3]);
		ChangePosition(mesh, trans[0], trans[1], trans[2]);
		//par_shapes_rotate(p_mesh, rd, axs);

		this->mesh = mesh;
		for (uint i = 0; i < 3; ++i)
		{
			translation[i] += trans[i];
			scale[i] += scle[i];
			axis[i] = axs[i];
		}
		rad = +rd;

		if (rad >= 360)
			rad -= 360;
	}
	//if parent have childs apply the transformation in all of them 
	if (parent->children.size() != 0)
	{
		for (std::vector<GameObject*>::iterator it = parent->children.begin(); it != parent->children.end(); ++it)
		{
			Transform* comp = dynamic_cast<Transform*>((*it)->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
			comp->LoadTransformation(mesh, trans, scle, rd, axs);
		}
	}
}

void Transform::UnLoadTransformation()
{
	ChangeScale(mesh, scale[0], scale[1], scale[3]);
	ChangePosition(mesh, translation[0], translation[1], translation[2]);
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
	glScalef(x,y,z);
	/*for (uint i = 0; i < mesh->num_vertices; i += 3) {
		mesh->vertices[i] *= x;
		mesh->vertices[i + 1] *= y;
		mesh->vertices[i + 2] *= z;
	}*/

	/*if (mesh->normals && (x != y || x != z || y != z)) {
		x = 1.0f / x;
		y = 1.0f / y;
		z = 1.0f / z;

		for (int i = 0; i < mesh->num_vertices*3; i += 3) {
			mesh->normals[i] *= x;
			mesh->normals[i + 1] *= y;
			mesh->normals[i + 2] *= z;

			float lsqr = sqrt(mesh->normals[i] * mesh->normals[i] + mesh->normals[i+1] * mesh->normals[i+1] + mesh->normals[i+2] * mesh->normals[i+2]);
			if (lsqr > 0) {
				mesh->normals[i] *= 1.0f / lsqr;
				mesh->normals[i+1] *= 1.0f / lsqr;
				mesh->normals[i+2] *= 1.0f / lsqr;
			}
		}
	}*/
}

void Transform::ChangePosition(Geometry* mesh, float x, float y, float z)
{
	for (uint i = 0; i < mesh->num_vertices*3; i += 3) 
	{
		mesh->vertices[i] += x;
		mesh->vertices[i + 1] += y;
		mesh->vertices[i + 2] += z;
	}
}

