#include "Application.h"
#include "ModuleMesh.h"
#include <vector>
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"
#include "Image.h"

#pragma comment(lib, "DevIL/lib/x86/Release/ILU.lib")
#pragma comment(lib, "DevIL/lib/x86/Release/DevIL.lib")
#pragma comment(lib, "DevIL/lib/x86/Release/ILUT.lib")
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
	//Init DevIL libs
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);

	ILuint devilError = ilGetError();
	if (devilError != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError));
	}

	//Check DevIL version
	else if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION || iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION || ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
	{
		LOG("DevIL version is different!\n");
	}
	else LOG("DevIL succesfully loaded");

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
	float* texture_coords = nullptr;
	int text_id = 0;
	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_Quality);


		
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			Geometry* data = new Geometry(nullptr);
			//Load vertex
			data->num_vertices = scene->mMeshes[i]->mNumVertices;
			data->vertices = new float[data->num_vertices * 3];
			memcpy(data->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * data->num_vertices * 3);
			LOG("New mesh with %d vertices", data->vertices);

			//load index
			if (scene->mMeshes[i]->HasFaces())
			{
				data->num_indices = scene->mMeshes[i]->mNumFaces * 3;
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
					data->normals = new float[scene->mMeshes[i]->mNumVertices * 3];
					memcpy(data->normals, scene->mMeshes[i]->mNormals, sizeof(float) * scene->mMeshes[i]->mNumVertices * 3);
				}

			}
			//Search for textures
			if (scene->HasMaterials())
			{
				data->texture = new Image(nullptr);
				data->texture->LoadCoords(scene->mMeshes[i]);
				data->texture->LoadMatirials(scene, file_name);
			}
			Geometry* geo = new Geometry(data, nullptr);
			geometry.push_back(geo);
			LOG("New mesh created from %s", file_name);
			App->camera->GoAroundGeometry(geo);
		}
		aiReleaseImport(scene);

	}
	else
		LOG("Error loading scene %s", file_name);
	
	return ret;
}

GLuint ModuleMesh::LoadTexture(const char * p_tex)
{
	//Gen image
	ILuint img_id = 0;
	ilGenImages(1, &img_id);
	ilBindImage(img_id);

	//load from path
	ilLoadImage(p_tex);



	ILuint devilError1 = ilGetError();
	if (devilError1 != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError1));
		return 0;
	}

	// If the image is flipped
	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);



	ILuint devilError2 = ilGetError();
	if (devilError2 != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError2));
		return 0;
	}

	//Send texture to GPU
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &img_id);
	glBindTexture(GL_TEXTURE_2D, img_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
		0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	ILuint devilError3 = ilGetError();
	if (devilError3 != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError3));
		return 0;
	}

	return img_id;
}

float ModuleMesh::TriangleCenterAxis(const float & p1, const float & p2, const float & p3)
{
	float middle_point = p1 - p2;

	return (middle_point + p3)*0.5;
}

