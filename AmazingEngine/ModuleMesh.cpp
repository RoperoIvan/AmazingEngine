#include "Application.h"
#include "ModuleMesh.h"
#include <vector>
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

ModuleMesh::ModuleMesh(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleMesh::~ModuleMesh()
{
}

bool ModuleMesh::Init()
{
	bool ret = true;
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

update_status ModuleMesh::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleMesh::CleanUp()
{
	for (std::vector<Geometry*>::iterator it = geometry.begin(); it != geometry.end(); it++)
	{
		if ((*it) != nullptr)
			delete (*it);
		(*it) = nullptr;
	}
	aiDetachAllLogStreams();
	return true;
}

bool ModuleMesh::LoadFile(const char * file_name)
{
	bool ret = false;

	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_Quality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			Geometry* data = new Geometry();
			//Load vertex
			data->num_vertices = scene->mMeshes[i]->mNumVertices;
			data->vertices = new float[data->num_vertices * 3];
			memcpy(data->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * data->num_vertices * 3);
			LOG("New mesh with %d vertices", data->vertices);

			//load index
			if (scene->mMeshes[i]->HasFaces())
			{	
				data->num_indices = scene->mMeshes[i]->mNumFaces*3;
				data->indices = new uint[data->num_indices * 3];
				for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
						memcpy(&data->indices[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(uint));
				}
				//load normals
				if (scene->mMeshes[i]->HasNormals())
				{
					for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
					{
						data->num_normals = scene->mMeshes[i]->mNumFaces * 2;
						data->normals = new float[data->num_normals * 3];

						aiVector3D *vertx1 = new aiVector3D( data->indices[j * 9],data->indices[j * 3 + 1],data->indices[j * 3 + 2] );
						aiVector3D *vertx2 = new aiVector3D(data->indices[j * 9 + 3],data->indices[j * 3 + 4],data->indices[j * 3 + 5] );
						aiVector3D *vertx3 = new aiVector3D(data->indices[j * 9 + 6],data->indices[j * 3 + 7],data->indices[j * 3 + 8] );

						aiVector3D *triangle_middle_point = new aiVector3D (TriangleCenterAxis(vertx1->x,vertx2->x,vertx3->x),TriangleCenterAxis(vertx1->y,vertx2->y,vertx3->y),TriangleCenterAxis(vertx1->z,vertx2->z,vertx3->z));
						
						memcpy(&data->normals[j * 6], triangle_middle_point, 3 * sizeof(float));

						aiVector3D *normal_point = new aiVector3D(*triangle_middle_point + *scene->mMeshes[i]->mNormals);

						memcpy(&data->normals[j * 6 + 3], &triangle_middle_point, 3 * sizeof(float));

						delete[] vertx1;
						vertx1 = nullptr;
						delete[] vertx2;
						vertx1 = nullptr;
						delete[] vertx3;
						vertx1 = nullptr;
					}
				}
			}
			Geometry* geo = new Geometry(data);
			geometry.push_back(geo);
			LOG("New mesh created from %s", file_name);
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", file_name);
	
	return ret;
}

float ModuleMesh::TriangleCenterAxis(const float & p1, const float & p2, const float & p3)
{
	float middle_point = p1 - p2;

	return (middle_point + p3)*0.5;
}

