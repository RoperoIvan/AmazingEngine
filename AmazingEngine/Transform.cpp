#include "Transform.h"
#include "Globals.h"
#include "GameObject.h"
#include "Geometry.h"
#include "par_shapes.h"
#include "MathGeoLib/include/MathGeoLib.h"
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
		//ChangeScale(mesh, scle[0], scle[1], scle[3]);
		ChangePosition(mesh, trans[0], trans[1], trans[2]);
		Rotate(mesh, rd, axs);
		mesh->ActualitzateBuffer();

		this->mesh = mesh;
		for (uint i = 0; i < 3; ++i)
		{
			translation[i] += trans[i];
			scale[i] += scle[i];
		}
		rad = +rd;
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
	//ChangeScale(mesh, scale[0], scale[1], scale[3]);
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
