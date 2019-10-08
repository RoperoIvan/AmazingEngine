#include "Geometry.h"

Geometry::Geometry(float* ver, uint* ind, float* norm, uint num_vert, uint num_ind, uint num_norm)
	: vertices(ver), indices(ind),normals(norm), num_vertices(num_vert), num_indices(num_ind),num_normals(num_norm)
{
	glGenBuffers(1, (uint*)&(id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*)&(id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_indices, indices, GL_STATIC_DRAW);

	//glGenBuffers(1, (uint*)&(id_normals));
	//glBindBuffer(GL_ARRAY_BUFFER, id_normals);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(int) * num_normals, normals, GL_STATIC_DRAW);
}
Geometry::Geometry(Geometry * geo) 
	: vertices(geo->vertices), indices(geo->indices), normals(geo->normals), num_vertices(geo->num_vertices), num_indices(geo->num_indices), num_normals(geo->num_normals)
{
	glGenBuffers(1, (uint*)&(id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*)&(id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_indices, indices, GL_STATIC_DRAW);

	//glGenBuffers(1, (uint*)&(id_normals));
	//glBindBuffer(GL_ARRAY_BUFFER, id_normals);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_normals * 3, normals, GL_STATIC_DRAW);
}
Geometry::Geometry()
{
}
Geometry::~Geometry()
{
	delete[] vertices;
	delete[] indices;
}

void Geometry::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);


	/*glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_normals);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, num_normals * 3);

	glDisableClientState(GL_VERTEX_ARRAY);*/
}

void Geometry::DebugDraw()
{
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	for (uint i = 0; i < num_vertices * 3; i += 3)
	{
		glColor3f(3.0f, 0.0f, 1.0f);
		glBegin(GL_LINES);
		glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
		glVertex3f(vertices[i] + normals[i]*2, vertices[i + 1] + normals[i + 1]*2, vertices[i + 2] + normals[i + 2]*2);
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
}