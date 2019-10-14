#include "Geometry.h"
#include "par_shapes.h"

//Constructor based in its variables
Geometry::Geometry(float* ver, uint* ind, float* norm, uint num_vert, uint num_ind, uint num_norm, GameObject* parent): Component(parent,COMPONENT_TYPE::COMPONENT_MESH),
	vertices(ver), indices(ind),normals(norm), num_vertices(num_vert), num_indices(num_ind), num_normals(num_norm)
{
	glGenBuffers(1, (uint*)&(id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*)&(id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_indices, indices, GL_STATIC_DRAW);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		LOG("Error Storing Indices! %s\n", gluErrorString(error));


}
//Constructor based on a geometry
Geometry::Geometry(Geometry* geo, GameObject* parent) : Component(parent, COMPONENT_TYPE::COMPONENT_MESH),
	vertices(geo->vertices), indices(geo->indices), normals(geo->normals), num_vertices(geo->num_vertices),num_indices(geo->num_indices), num_normals(geo->num_normals), texture(geo->texture)
{
	glGenBuffers(1, (uint*) & (id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*) & (id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_indices, indices, GL_STATIC_DRAW);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		LOG("Error Storing Indices! %s\n", gluErrorString(error));

	//Alloc the textures for future use
	if (texture != nullptr)
	{
		if (texture->texture_id != 0) 
		{
			glGenBuffers(1, (uint*) & (texture->id_coords));
			glBindBuffer(GL_ARRAY_BUFFER, texture->id_coords);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 2, texture->uv_coord, GL_STATIC_DRAW);
			error = glGetError();
			if (error != GL_NO_ERROR)
				LOG("Error Storing textures! %s\n", gluErrorString(error));
		}
	}
}
//Primitives constructor
Geometry::Geometry(float* ver, uint* ind, float* normals, int num_vert, int num_ind, float r, float g, float b, float a, GameObject* parent) : Component(parent, COMPONENT_TYPE::COMPONENT_MESH),
vertices(ver), indices(ind), normals(normals),num_vertices(num_vert), par_num_indices(num_ind), r(r), g(g), b(b), a(a)
{
	glGenBuffers(1, &id_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vert * 3, ver, GL_STATIC_DRAW);

	glGenBuffers(1, &id_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T) * par_num_indices * 3, ind, GL_STATIC_DRAW);

}

Geometry::Geometry(GameObject* parent):Component(parent, COMPONENT_TYPE::COMPONENT_MESH)
{
}
Geometry::~Geometry()
{
}
//Draw geometry based in meshes
void Geometry::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	if (texture != nullptr)
	{
		if (texture->texture_id != 0)
		{
			//Bind textures
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, texture->texture_id);
			glBindBuffer(GL_ARRAY_BUFFER, texture->id_coords);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}
	}
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
//Draw primitives geometries
void Geometry::DrawPrimitives()
{
	glColor4f(r, g, b, a);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, par_num_indices * 3, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);
}
//DebugDraw for all geometries
void Geometry::DebugDraw()
{//TODO: Fix normals in the primitive geometries
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	for (uint i = 0; i < num_vertices * 3; i += 3)
	{
		glColor3f(3.0f, 0.0f, 1.0f);
		glBegin(GL_LINES);
		glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
		glVertex3f(vertices[i] + normals[i]*10, vertices[i + 1] + normals[i + 1]*10, vertices[i + 2] + normals[i + 2]*10);
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
}

void Geometry::Enable()
{
}

void Geometry::Update()
{
}

void Geometry::Disable()
{
}
