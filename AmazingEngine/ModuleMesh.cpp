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

#pragma comment(lib, "DevIL/libx86/ILU.lib")
#pragma comment(lib, "DevIL/libx86/DevIL.lib")
#pragma comment(lib, "DevIL/libx86/ILUT.lib")
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
	if (App->guiManager->debug_draw)
	{
		glBegin(GL_LINES);
		DrawBoundingBoxes();
		glEnd();
	}

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

	std::string p_file = file_name;
	std::string format;

	LOG("Loading file from %s", file_name);

	//We want to get the format so we read the path from the end
	while (p_file.back() != '.')
	{
		format.push_back(p_file.back());
		p_file.pop_back();
	}
	std::transform(format.begin(), format.end(), format.begin(), ::tolower);
	//Since its backwards we reverse it to then compare it with the formats that the engine supports
	std::reverse(format.begin(), format.end());
	if (format.compare("fbx") == 0)
	{
		LOG("FORMAT IS FBX");
		LoadFBXFile(file_name);
	}
	else if (format.compare("png") == 0 || format.compare("dds") == 0 || format.compare("jpg") == 0)
	{
		LOG("FORMAT IS AN IMAGE");
		LoadTextureFile(file_name);
	}
	else LOG("File format '.%s' not recognized", format.c_str());

	
	return ret;
}

bool ModuleMesh::LoadFBXFile(const char * file_name)
{
	bool ret = true;
	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_Quality);

	if (scene != nullptr && scene->HasMeshes())
	{
		GameObject* newfbx = new GameObject();
		newfbx->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM);
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			GameObject* game_object = new GameObject(newfbx);

			Geometry* data = dynamic_cast<Geometry*>(game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MESH));
			data->LoadData(scene->mMeshes[i]);

			data->transform = dynamic_cast<Transform*>(game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
			data->transform->Init(data->vertices[0], data->vertices[1], data->vertices[2]);

			
			if (scene->HasMaterials())
			{
				Image* tex = dynamic_cast<Image*>(game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MATERIAL));
				if (tex->LoadMaterials(scene, file_name))
					App->scene->textures.push_back(tex);
				
				data->texture = tex;
				
			}
			LOG("New mesh created from %s", file_name);

			newfbx->children.push_back(game_object);
		}
		App->scene->game_objects.push_back(newfbx);
		
		aiReleaseImport(scene);

	}
	else
		LOG("Error loading scene %s", file_name);


	return ret;
}

bool ModuleMesh::LoadTextureFile(const char * file_name)
{
	bool ret = true;
	bool is_loaded = false;
	Image* tex = nullptr;
	for (std::vector<Image*>::iterator iter = App->scene->textures.begin(); iter != App->scene->textures.end(); ++iter)
	{
		if (((*iter)->p_tex.compare(file_name)) == 0)
		{
			is_loaded = true;
			tex = *iter;

			break;
		}
	}
	if (App->scene->game_object_select != nullptr)
	{
		ChangeTex(App->scene->game_object_select , file_name, tex);
	}
	else
	{
		if (!is_loaded)
		{
			tex = new Image(nullptr);
			tex->texture_id = tex->LoadImages(file_name);
			tex->p_tex.assign(file_name);
			App->scene->textures.push_back(tex);
		}
	}
	return ret;
}

void ModuleMesh::ChangeTex(GameObject* object, const char* file_name, Image* texture)
{
	if (object->children.empty())
	{
		for (std::vector<Component*>::iterator iter = object->components.begin(); iter != object->components.end(); ++iter)
		{
			if ((*iter)->type == COMPONENT_TYPE::COMPONENT_MATERIAL)
			{
				Image* tex = dynamic_cast<Image*>(*iter);
				if (texture == nullptr)
				{
					tex->texture_id = tex->LoadImages(file_name);
					tex->tmp_id = tex->texture_id;
					tex->p_tex.assign(file_name);
					App->scene->textures.push_back(tex);
				}
				else
				{
					tex = texture;
					
				}
				for (std::vector<Component*>::iterator it = object->components.begin(); it != object->components.end(); ++it)
				{
					if ((*it)->type == COMPONENT_TYPE::COMPONENT_MESH)
					{
						Geometry* mesh = dynamic_cast<Geometry*>(*it);
						mesh->texture = tex;
						break;
					}
				}
				break;
			}
		}
	}
	else
	{
		for (std::vector<GameObject*>::iterator iter = object->children.begin(); iter != object->children.end(); ++iter)
			ChangeTex(*iter,file_name, texture);
	}
}

float ModuleMesh::TriangleCenterAxis(const float & p1, const float & p2, const float & p3)
{
	float middle_point = p1 - p2;

	return (middle_point + p3)*0.5;
}

void ModuleMesh::DrawBoundingBoxes()
{
	glLineWidth(0.2);
	glColor3f(204, 255, 0.0f);

	while (b_boxes.empty() == false)
	{
		//We create the lines of the cube
		for (int i = 0; i < 4; i++)
		{
			glVertex3f(b_boxes.front()->CornerPoint(i + 4).x, b_boxes.front()->CornerPoint(i + 4).y, b_boxes.front()->CornerPoint(i + 4).z);
			glVertex3f(b_boxes.front()->CornerPoint(i).x, b_boxes.front()->CornerPoint(i).y, b_boxes.front()->CornerPoint(i).z);
		}
		//Create the vertices that define the cubes faces that form the mesh box
		for (int i = 0; i <= 4; i += 4)
		{
			glVertex3f(b_boxes.front()->CornerPoint(i).x, b_boxes.front()->CornerPoint(i).y, b_boxes.front()->CornerPoint(i).z);
			glVertex3f(b_boxes.front()->CornerPoint(i + 1).x, b_boxes.front()->CornerPoint(i + 1).y, b_boxes.front()->CornerPoint(i + 1).z);

			glVertex3f(b_boxes.front()->CornerPoint(i + 2).x, b_boxes.front()->CornerPoint(i + 2).y, b_boxes.front()->CornerPoint(i + 2).z);
			glVertex3f(b_boxes.front()->CornerPoint(i + 3).x, b_boxes.front()->CornerPoint(i + 3).y, b_boxes.front()->CornerPoint(i + 3).z);

			glVertex3f(b_boxes.front()->CornerPoint(i).x, b_boxes.front()->CornerPoint(i).y, b_boxes.front()->CornerPoint(i).z);
			glVertex3f(b_boxes.front()->CornerPoint(i + 2).x, b_boxes.front()->CornerPoint(i + 2).y, b_boxes.front()->CornerPoint(i + 2).z);

			glVertex3f(b_boxes.front()->CornerPoint(i + 1).x, b_boxes.front()->CornerPoint(i + 1).y, b_boxes.front()->CornerPoint(i + 1).z);
			glVertex3f(b_boxes.front()->CornerPoint(i + 3).x, b_boxes.front()->CornerPoint(i + 3).y, b_boxes.front()->CornerPoint(i + 3).z);
		}
		//We pop from the queue because we need to maintain update it for future changes in the gameobject's bounding box 
		b_boxes.pop();
	}
}
