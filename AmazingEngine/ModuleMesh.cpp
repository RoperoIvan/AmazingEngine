#include "ModuleMesh.h"
#include <vector>
#include "glew/include/GL/glew.h"
#include"ImGui/imgui_impl_opengl3.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

ModuleMesh::ModuleMesh()
{
}

ModuleMesh::~ModuleMesh()
{
}

bool ModuleMesh::Init()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

update_status ModuleMesh::PostUpdate(float dt)
{
	return update_status();
}

bool ModuleMesh::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

bool ModuleMesh::LoadFile(const char * file_name)
{
	bool ret = false;

	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			Mesh_data *mesh = new Mesh_data();
			mesh->num_vertex = scene->mMeshes[i]->mNumVertices * 3;
			uint k = 0;
			for (int j = 0; j < scene->mMeshes[i]->mNumVertices; ++j)
			{
				mesh->vertex[k] = scene->mMeshes[i]->mVertices->x;
				++k;
				mesh->vertex[k] = scene->mMeshes[i]->mVertices->y;
				++k;
				mesh->vertex[k] = scene->mMeshes[i]->mVertices->z;
				++k;
			}
			if (scene->mMeshes[i]->HasFaces())
			{
				mesh->num_index = scene->mMeshes[i]->mNumFaces * 3;
				for (uint i = 0; i < scene->mMeshes[i]->mNumFaces; ++i)
				{
					if (scene->mMeshes[i]->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
						memcpy(&mesh->index[i * 3], scene->mMeshes[i]->mFaces[i].mIndices, 3 * sizeof(uint));
				}
			}
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", file_name);

	return ret;
}
