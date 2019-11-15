#include "Geometry.h"
#include <vector>
#include "ImGui/imgui.h"
#include "Assimp/include/scene.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include <fstream>

//Primitives constructor

Geometry::Geometry(GameObject* parent):Component(parent, COMPONENT_TYPE::COMPONENT_MESH)
{
	r = g = b = a = 255;
	is_enable = true;
}
Geometry::~Geometry()
{

}

void Geometry::Disable()
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

void Geometry::CreatePrimitive(par_shapes_mesh* p_mesh, float col0, float col1, float col2, float col3)
{
	num_vertices = p_mesh->npoints;
	par_num_indices = p_mesh->ntriangles;
	num_indices = p_mesh->ntriangles * 3;
	num_coords = p_mesh->npoints * 2;

	vertices = new float[num_vertices*3];
	indices = new uint[num_indices];
	normals = new float[num_vertices*3];
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

	transform = dynamic_cast<Transform*>(parent->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	texture = dynamic_cast<Image*>(parent->CreateComponent(COMPONENT_TYPE::COMPONENT_MATERIAL));
	
	LoadBuffers();

}


//DebugDraw for all geometries
void Geometry::DebugDraw()
{
	if (parent->show_vertices_normals && num_normals != 0)
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
	if (parent->show_face_normals && num_normals != 0)
	{
		glEnableClientState(GL_VERTEX_ARRAY);

		for (uint i = 0; i <  num_face_normals; i += 6)
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

void Geometry::Update()
{
	if (App->guiManager->frustum_culling)
	glPushMatrix();
	glMultMatrixf((GLfloat*)&transform->global_matrix.Transposed());
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(r, g, b, a);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	if (texture != nullptr)
	{
		if (App->mesh->IsCulling(this))
		{
			glPushAttrib(GL_CURRENT_BIT);
			glColor4f(r, g, b, a);
			glEnableClientState(GL_VERTEX_ARRAY);

			glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
			glVertexPointer(3, GL_FLOAT, 0, NULL);
			if (texture != nullptr)
			{
				if (texture->texture_id != 0 && texture->show)
				{
					//Bind textures
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glBindTexture(GL_TEXTURE_2D, 0);
					glBindTexture(GL_TEXTURE_2D, texture->texture_id);
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

			DebugDraw();


		}
	}
	else
	{
		glPushMatrix();
		glMultMatrixf((GLfloat*)&transform->global_matrix.Transposed());
		glPushAttrib(GL_CURRENT_BIT);
		glColor4f(r, g, b, a);
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		if (texture != nullptr)
		{
			if (texture->texture_id != 0 && texture->show)
			{
				//Bind textures
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindTexture(GL_TEXTURE_2D, texture->texture_id);
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

		DebugDraw();


	}
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopAttrib();
	glPopMatrix();
	DebugDraw();
}

void Geometry::LoadData(aiMesh* mesh)
{
	//Load vertex	
	num_vertices = mesh->mNumVertices;
	vertices = new float[num_vertices * 3];
	memcpy(vertices, mesh->mVertices, sizeof(float) * num_vertices * 3);
	LOG("New mesh with %d vertices", vertices);

	//load index
	if (mesh->HasFaces())
	{	
		num_indices = mesh->mNumFaces * 3;
		indices = new uint[num_indices * 3];
		for (uint j = 0; j < mesh->mNumFaces; ++j)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
			}
			else
				memcpy(&indices[j * 3], mesh->mFaces[j].mIndices, 3 * sizeof(uint));
		}
		//load normals
		if (mesh->HasNormals())
		{
			num_normals = mesh->mNumVertices * 3;
			normals = new float[mesh->mNumVertices * 3];
			memcpy(normals, mesh->mNormals, sizeof(float) * mesh->mNumVertices * 3);
		}

		num_face_normals = num_vertices * 3;
		face_normals = new float[num_face_normals];
		uint j = 0;
		for (uint i = 0; i < num_vertices*3; i += 9)
		{
			float u[3] = { (vertices[i + 3] - vertices[i + 0]),(vertices[i + 4] - vertices[i + 1]),(vertices[i + 5] - vertices[i + 2]) };
			float v[3] = { (vertices[i + 6] - vertices[i + 3]),(vertices[i + 7] - vertices[i + 4]),(vertices[i + 8] - vertices[i + 5]) };
			face_normals[j] = (vertices[i] + vertices[i + 3] + vertices[i + 6])/3;
			face_normals[j + 1] = (vertices[i + 1] + vertices[i + 4] + vertices[i + 7]) / 3;;
			face_normals[j + 2] = (vertices[i + 2] + vertices[i + 5] + vertices[i + 8]) / 3;;
			face_normals[j + 3] = face_normals[j] + (u[1] * v[2] - u[2] * v[1]);
			face_normals[j + 4] = face_normals[j + 1] + (u[2] * v[0] - u[0] * v[2]);
			face_normals[j + 5] = face_normals[j + 2] + (u[0] * v[1] - u[1] * v[0]);
			j += 6;
		}
	}

	if (mesh->HasTextureCoords(0))
	{
		num_coords = mesh->mNumVertices * 2;
		uv_coord = new float[num_coords];
		for (uint i = 0; i < mesh->GetNumUVChannels(); ++i)
		{
			for (uint k = 0; k < mesh->mNumVertices; ++k) {
				uv_coord[k * 2] = mesh->mTextureCoords[i][k].x;
				uv_coord[k * 2 + 1] = mesh->mTextureCoords[i][k].y;
				/*LOG("Texture coords: %f", texture_coords[k]);*/
			}
		}
	}

	//Adapt bounding box to geometry-----------------
	if (parent != nullptr)
		CalculateParentBoundingBox(parent);
	
	LoadBuffers();
}

void Geometry::ActualitzateBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);
}

void Geometry::Save(FILE* file)
{
	fputs("<vertices> \n", file);
	for (int i = 0; i < num_vertices * 3; ++i)
	{
		fprintf(file, "%i = '%s' ",i, std::to_string(vertices[i]).c_str());
		
	}
	fputs("\n</vertices>\n", file);
	fputs("<indices>\n", file);
	for (int i = 0; i < num_indices; ++i)
	{
		fprintf(file, "%i = '%s' ",i, std::to_string(indices[i]).c_str());
	}
	fputs("\n</indices>\n", file);
}

void Geometry::CalculateParentBoundingBox(GameObject* object)
{
	std::vector <float3> vertex_array;
	if (vertices == nullptr)
		return;
	for (int i = 0; i < num_indices * 3; i += 3)
		vertex_array.push_back(float3(vertices[i], vertices[i + 1], vertices[i + 2]));

	object->bounding_box.Enclose(&vertex_array[0], (int)num_vertices);

	if (object->parent != nullptr)
	{
		CalculateParentBoundingBox(object->parent);
	}
}

void Geometry::LoadBuffers()
{
	glGenBuffers(1, (uint*) & (id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*) & (id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_indices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, (uint*) & (id_coords));
	glBindBuffer(GL_ARRAY_BUFFER, id_coords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_coords, uv_coord, GL_STATIC_DRAW);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		LOG("Error Storing Indices! %s\n", gluErrorString(error));

}

