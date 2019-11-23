#include "Geometry.h"
#include <vector>
#include "ImGui/imgui.h"
#include "Assimp/include/scene.h"
#include "GameObject.h"
#include "Application.h"
#include "FileSystem.h"
#include "ModuleInput.h"
#include <fstream>

//Primitives constructor

Geometry::Geometry(GameObject* parent):Component(parent, COMPONENT_TYPE::COMPONENT_MESH)
{
	is_enable = true;
}
Geometry::~Geometry()
{

}

//DebugDraw for all geometries
void Geometry::DebugDraw()
{
	if (parent->show_vertices_normals)
	{
		r_mesh->DebugDraw(true, true);
	}
}

void Geometry::Update()
{
	
}

void Geometry::SetMesh(ResourceMesh * m)
{
		if (r_mesh)
			r_mesh->DecreaseInstancies();

		r_mesh = m;

		if (r_mesh)
			r_mesh->IncreaseInstancies();
}
