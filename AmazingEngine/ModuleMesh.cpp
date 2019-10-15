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
#include "GameObject.h"
#include "ModuleScene.h"

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
		GameObject* game_object = new GameObject();
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			Component* data = game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MESH);
			
			dynamic_cast<Geometry*>(data)->LoadData(scene->mMeshes[i]);
			
			if (scene->HasMaterials())
			{
				Component* tex = game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MATERIAL);
				dynamic_cast<Image*>(tex)->LoadCoords(scene->mMeshes[i]);
				dynamic_cast<Image*>(tex)->LoadMatirials(scene, file_name);
				dynamic_cast<Geometry*>(data)->texture = dynamic_cast<Image*>(tex);
				
			}
			LOG("New mesh created from %s", file_name);

		}
		App->scene->game_objects.push_back(game_object);
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

