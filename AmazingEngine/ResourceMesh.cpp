#include "ResourceMesh.h"



ResourceMesh::ResourceMesh(uint id) : Resource(id, TYPE::MESH)
{
	r = g = b = a = 255;
}


ResourceMesh::~ResourceMesh()
{
}

void ResourceMesh::SetData(float * ver, uint * ind, uint num_vert, uint num_ind, float * normal, float * texture_coords, uint num_coord, uint num_face_normal, uint id_coord)
{
	vertices = ver;
	indices = ind;
	num_vertices = num_vert;
	num_indices = num_ind;
	normals = normal;
	uv_coord = texture_coords;
	num_coords = num_coord;
	num_face_normals = num_face_normal;
	id_coords = id_coord;

	LoadBuffers();
}

void ResourceMesh::Disable()
{
	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;
	delete[] normals;
	normals = nullptr;
	delete[] face_normals;
	face_normals = nullptr;
}

void ResourceMesh::ActualitzateBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);
}

void ResourceMesh::DrawMesh()
{
	glPushMatrix();
	if (transform != nullptr)
		glMultMatrixf((GLfloat*)&transform->global_matrix.Transposed());
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(r, g, b, a);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	if (texture != nullptr)
	{
		if (texture->r_texture->texture_id != 0 && texture->show)
		{
			//Bind textures
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, texture->r_texture->texture_id);
			glBindBuffer(GL_ARRAY_BUFFER, id_coords);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}
	}
	else
		glColor4f(r, g, b, a);

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopAttrib();
	glPopMatrix();
	/*DebugDraw();*/
}

void ResourceMesh::DebugDraw(bool vert, bool face)
{
	if (vert && num_normals != 0)
	{
		glEnableClientState(GL_VERTEX_ARRAY);

		for (uint i = 0; i < num_vertices * 3; i += 3)
		{
			glColor3f(3.0f, 0.0f, 1.0f);
			glBegin(GL_LINES);
			glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
			glVertex3f(vertices[i] + normals[i] * 2, vertices[i + 1] + normals[i + 1] * 2, vertices[i + 2] + normals[i + 2] * 2);
			glEnd();
			glColor3f(1.0f, 1.0f, 1.0f);
		}

		glDisableClientState(GL_VERTEX_ARRAY);
	}
	if (face && num_normals != 0)
	{
		glEnableClientState(GL_VERTEX_ARRAY);

		for (uint i = 0; i < num_face_normals; i += 6)
		{
			glColor3f(3.0f, 0.0f, 1.0f);
			glBegin(GL_LINES);
			glVertex3f(face_normals[i], face_normals[i + 1], face_normals[i + 2]);
			glVertex3f(face_normals[i + 3], face_normals[i + 4], face_normals[i + 5]);
			glEnd();
			glColor3f(1.0f, 1.0f, 1.0f);
		}

		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void ResourceMesh::LoadBuffers()
{
	//LoadBuffers

	glGenBuffers(1, (uint*) & (id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*) & (id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_indices, indices, GL_STATIC_DRAW);

	if (uv_coord != nullptr)
	{
		glGenBuffers(1, (uint*) & (id_coords));
		glBindBuffer(GL_ARRAY_BUFFER, id_coords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_coords, uv_coord, GL_STATIC_DRAW);
	}	

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		LOG("Error Storing Indices! %s\n", gluErrorString(error));
}

void ResourceMesh::CalculateParentBoundingBox(GameObject * object)
{
	std::vector <float3> vertex_array;
	if (vertices == nullptr)
		return;
	for (int i = 0; i < num_vertices * 3; i += 3)
		vertex_array.push_back(float3(vertices[i], vertices[i + 1], vertices[i + 2]));

	object->bounding_box->aabb.Enclose(&vertex_array[0], (int)num_vertices);
	object->bounding_box->obb.SetNegativeInfinity();
	object->bounding_box->obb = object->bounding_box->aabb.ToOBB();
	if (object->parent != nullptr)
	{
		CalculateParentBoundingBox(object->parent);
	}
}

void ResourceMesh::CreatePrimitive(par_shapes_mesh * p_mesh, Geometry* geo, float col0, float col1, float col2, float col3)
{
	num_vertices = p_mesh->npoints;
	par_num_indices = p_mesh->ntriangles;
	num_indices = p_mesh->ntriangles * 3;
	num_coords = p_mesh->npoints * 2;

	vertices = new float[num_vertices * 3];
	indices = new uint[num_indices];
	normals = new float[num_vertices * 3];
	uv_coord = new float[num_coords];
	memcpy(vertices, p_mesh->points, sizeof(float) * num_vertices * 3);
	memcpy(indices, p_mesh->triangles, sizeof(uint) * num_indices);
	if (p_mesh->normals != NULL)
	{
		memcpy(normals, p_mesh->normals, sizeof(float) * num_vertices * 3);
		num_normals = num_vertices * 3;
	}
	if (p_mesh->tcoords != nullptr)
	{
		memcpy(uv_coord, p_mesh->tcoords, sizeof(float) * num_coords);
	}

	r = col0;
	g = col1;
	b = col2;
	a = col3;

	transform = dynamic_cast<Transform*>(geo->parent->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	texture = dynamic_cast<Image*>(geo->parent->CreateComponent(COMPONENT_TYPE::COMPONENT_MATERIAL));
	CalculateParentBoundingBox(geo->parent);
	LoadBuffers();
}
