#include "Application.h"
#include "ModuleMesh.h"
#include <vector>
#include <fstream>
#include <experimental\filesystem>
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

		AddFrustumBox(&App->scene->current_camera->frustum);
		/*DrawFrustums();*/
		DrawBoundingBoxes();
		DrawRay();
		if(App->guiManager->active_octree)
			App->scene->octree->Draw();
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
	//else if (format.compare("png") == 0 || format.compare("dds") == 0 || format.compare("jpg") == 0)
	//{
	//	LOG("FORMAT IS AN IMAGE");
	//	LoadTextureFile(file_name);
	//}
	else LOG("File format '.%s' not recognized", format.c_str());

	
	return ret;
}

bool ModuleMesh::LoadFBXFile(const char * file_name)
{
	bool ret = true;
	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_Quality);

	if (scene != nullptr)
	{
		if (scene->HasMaterials())
		{ 
			LoadTemporalMaterials(scene);
		}
		if (scene->HasMeshes())
		{
			GameObject* newfbx = new GameObject();
			LoadObjects(scene->mRootNode, scene, file_name, newfbx);
			App->scene->game_objects.push_back(newfbx);
			App->scene->octree->Insert(newfbx);
		}
		if (scene->HasCameras())
		{
			for (uint i = 0; i < scene->mNumCameras; ++i)
			{
				GameObject* camera = App->scene->CreateCamera();
				Camera* cam = (Camera*)camera->GetComponentByType(COMPONENT_TYPE::COMPONENT_CAMERA);
				
				Transform* transform = (Transform*)camera->GetComponentByType(COMPONENT_TYPE::COMPONENT_TRANSFORM);
				aiVector3D ai_location;
				aiVector3D ai_scale;
				aiQuaternion ai_rotation;
				aiMatrix4x4 matrix;
				scene->mCameras[i]->GetCameraMatrix(matrix);
				matrix.Decompose(ai_scale, ai_rotation, ai_location);
				float4x4 rot_matrix = math::float4x4::FromTRS(float3(ai_location[0], ai_location[1], ai_location[2]),
					Quat(ai_rotation.x, ai_rotation.y, ai_rotation.z, ai_rotation.w),
					float3(ai_scale[0], ai_scale[1], ai_scale[2]));
				transform->Init(rot_matrix);
			}
		}
		aiReleaseImport(scene);
		tmp_material.clear();
	}
	else
		LOG("Error loading scene %s", file_name);

	rand_id = 0;

	return ret;
}

void ModuleMesh::LoadObjects(aiNode* node, const aiScene* scene, const char*& file_name, GameObject*& parent)
{
	//FIRST PARENT
	parent->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM);

	int index_material = 0;
	for (uint k = 0; k < node->mNumMeshes; ++k)
	{
		//CHILDREN
		GameObject* game_object = new GameObject(parent);

		//-----------------------------MESHLOAD--------------------------------------\\
						//FILLING COMPONENT MESH
		std::string g_name = LoadData(scene->mMeshes[node->mMeshes[k]]);
		//LOADING TO OUR FORMAT
		LoadMeshFromFormat(g_name.c_str(), game_object);
		Geometry* my_geo = dynamic_cast<Geometry*>(game_object->GetComponentByType(COMPONENT_TYPE::COMPONENT_MESH));

		//--------------------------TRANSFORMATION-----------------------------------\\

		my_geo->transform = dynamic_cast<Transform*>(game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		aiVector3D ai_location;
		aiVector3D ai_scale;
		aiQuaternion ai_rotation;
		node->mTransformation.Decompose(ai_scale,ai_rotation,ai_location);
		float4x4 rot_matrix = math::float4x4::FromTRS(float3(ai_location[0], ai_location[1], ai_location[2]),
			Quat(ai_rotation.x, ai_rotation.y, ai_rotation.z, ai_rotation.w),
			float3(ai_scale[0], ai_scale[1], ai_scale[2]));
		my_geo->transform->Init(rot_matrix);
		LOG("New mesh created from %s", file_name);

		//-----------------------------TEXTURELOAD-------------------------------------\\
						//Get the material index to then compare if this has been loaded before
		index_material = scene->mMeshes[node->mMeshes[k]]->mMaterialIndex;

		LoadMaterials(scene, game_object, file_name, index_material);
		LOG("New material created from %s", file_name);

		//------------------------------------------------------------------------------\\
						
		LOG("-----------------------------------------");
		parent->children.push_back(game_object);
		App->scene->octree->Insert(game_object);
	}
	//------------------------------CHILDRENS---------------------------------------\\
		
	for (uint i = 0; i < node->mNumChildren; ++i)
	{
		LoadObjects(node->mChildren[i], scene, file_name, parent);
	}
}

void ModuleMesh::ImportTextureToDDSFile(const char * file_name) const
{
	ILuint size;
	ILubyte *data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
	if (size > 0) {
		data = new ILubyte[size]; // allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
			App->file_system->SaveTextureToDDS((char*)data, (uint)size, file_name);
		delete[] data;
	}
}

bool ModuleMesh::IsCulling(Geometry * g)
{	
	bool ret = ContainsABB(g->GetParentObject()->bounding_box->aabb);
	return ret;
}

bool ModuleMesh::ContainsABB(const AABB & b_box)
{
	float3 b_corners[8];
	b_box.GetCornerPoints(b_corners);

	for (uint plane_sides = 0; plane_sides < 6; ++plane_sides)
	{
		int inside_corners = 8;
		for (uint point = 0; point < 8; ++point)
		{
			if (App->scene->current_camera->frustum.GetPlane(plane_sides).IsOnPositiveSide(b_corners[point]))
				--inside_corners;
		}
		// We dont need to know (at the moment) the exact number of corners that are in
		if (inside_corners == 0) //We just look if some corner is inside to cull or not
			return false;
	}
	return true;
}

std::string ModuleMesh::LoadData(aiMesh * mesh)
{
	//Create temporal data that we will fill
	uint num_vertices = 0;
	float* vertices = nullptr;
	uint num_indices = 0;
	uint* indices = nullptr;
	uint num_normals = 0;
	float* normals = nullptr;
	uint num_face_normals = 0;
	float* face_normals = nullptr;
	uint num_coords = 0;
	float* uv_coord = nullptr;
	uint id_coords = 0;

	//Vertices	
	num_vertices = mesh->mNumVertices;
	vertices = new float[num_vertices * 3];
	memcpy(vertices, mesh->mVertices, sizeof(float) * num_vertices * 3);
	LOG("New mesh with %d vertices", vertices);

	//Indices
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

		//Normals
		if (mesh->HasNormals())
		{
			num_normals = mesh->mNumVertices * 3;
			normals = new float[mesh->mNumVertices * 3];
			memcpy(normals, mesh->mNormals, sizeof(float) * mesh->mNumVertices * 3);
		}

		num_face_normals = num_vertices * 3;
		face_normals = new float[num_face_normals];
		uint j = 0;
		for (uint i = 0; i < num_vertices * 3; i += 9)
		{
			float u[3] = { (vertices[i + 3] - vertices[i + 0]),(vertices[i + 4] - vertices[i + 1]),(vertices[i + 5] - vertices[i + 2]) };
			float v[3] = { (vertices[i + 6] - vertices[i + 3]),(vertices[i + 7] - vertices[i + 4]),(vertices[i + 8] - vertices[i + 5]) };
			face_normals[j] = (vertices[i] + vertices[i + 3] + vertices[i + 6]) / 3;
			face_normals[j + 1] = (vertices[i + 1] + vertices[i + 4] + vertices[i + 7]) / 3;;
			face_normals[j + 2] = (vertices[i + 2] + vertices[i + 5] + vertices[i + 8]) / 3;;
			face_normals[j + 3] = face_normals[j] + (u[1] * v[2] - u[2] * v[1]);
			face_normals[j + 4] = face_normals[j + 1] + (u[2] * v[0] - u[0] * v[2]);
			face_normals[j + 5] = face_normals[j + 2] + (u[0] * v[1] - u[1] * v[0]);
			j += 6;
		}
	}
	//Coordinates
	if (mesh->HasTextureCoords(0))
	{
		num_coords = mesh->mNumVertices * 2;
		uv_coord = new float[num_coords];
		for (uint i = 0; i < mesh->GetNumUVChannels(); ++i)
		{
			for (uint k = 0; k < mesh->mNumVertices; ++k) {
				uv_coord[k * 2] = mesh->mTextureCoords[i][k].x;
				uv_coord[k * 2 + 1] = mesh->mTextureCoords[i][k].y;
			}
		}
	}

	//Generate random name if it doesn't have one
	std::string name = RandomName(mesh);
	
	//Copying and saving the mesh info into the mesh file on our library
	App->file_system->SaveMeshToFormat(name.c_str(), mesh->mNumVertices, mesh->mNumFaces * 3, vertices, indices, normals, num_normals, num_face_normals, uv_coord, num_coords, id_coords);

	return name;
}

void ModuleMesh::LoadMeshFromFormat(const char * file_name, GameObject* g_object)
{
	char* data = nullptr;

	//See if the file was correctly created
	if (App->file_system->LoadDataFromLibrary(&data, file_name, "Meshes", ".Amazing") == false)
	{
		return;
	}
	//num vertex - num index - vertex - index - has normals - has text coords - normals - text coords - num_normals - num_face_normals - num_coords - id_coords

	//Filling mesh variables
	uint num_vert = 0;
	uint num_ind = 0;
	uint num_normals = 0;
	uint num_face_normals = 0;
	uint num_coords = 0;
	uint id_coords = 0;
	bool b_normals = false;
	bool b_coords = false;

	//Look for mesh tag errors
	char* cursor = data;
	uint tag[] = { -1 };
	uint size_of = sizeof(uint);
	memcpy(tag, cursor, size_of);

	if (*tag != 2)
	{
		LOG("ERROR: this is not a mesh");
		return;
	}
	cursor += size_of;

	//UINTS
	uint ranges[6];
	size_of = sizeof(ranges);
	memcpy(ranges, cursor, size_of);
	num_vert = ranges[0];
	num_ind = ranges[1];
	num_normals = ranges[2];
	num_face_normals = ranges[3];
	num_coords = ranges[4];
	id_coords = ranges[5];
	cursor += size_of;

	//VERTICES
	float* vert = new float[num_vert * 3];
	size_of = sizeof(float)*num_vert * 3;
	memcpy(vert, cursor, size_of);
	cursor += size_of;

	//INDICES
	unsigned int* ind = new unsigned int[num_ind];
	size_of = sizeof(float)*num_ind;
	memcpy(ind, cursor, size_of);
	cursor += size_of;

	//BOOLS
	bool bools[2];

	float* normals = nullptr;
	float* texture_coord = nullptr;

	//NORMALS
	size_of = sizeof(bool);
	memcpy(&bools[0], cursor, size_of);
	cursor += size_of;
	if (bools[0])
	{
		normals = new float[num_vert * 3];
		size_of = sizeof(float) * num_vert * 3;
		memcpy(normals, cursor, size_of);
		cursor += size_of;
	}

	//COORDS
	size_of = sizeof(bool);
	memcpy(&bools[1], cursor, size_of);
	cursor += size_of;
	if (bools[1])
	{
		texture_coord = new float[num_vert * 2];
		size_of = sizeof(float) * num_vert * 2;
		memcpy(texture_coord, cursor, size_of);
		cursor += size_of;
	}

	LOG("Loaded %s mesh in our format complete", file_name);
	delete[] data;

	//Finally fill the new component with all the variables that we previously saved in our file 
	Geometry* mesh;
	mesh = dynamic_cast<Geometry*>(g_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MESH));
	mesh->vertices = vert;
	mesh->indices = ind;
	mesh->num_vertices = num_vert;
	mesh->num_indices = num_ind;
	mesh->normals = normals;
	mesh->uv_coord = texture_coord;
	mesh->num_coords = num_coords;
	mesh->num_face_normals = num_face_normals;
	mesh->num_coords = num_coords;
	mesh->id_coords = id_coords;
	mesh->name = file_name;
	mesh->transform = dynamic_cast<Transform*>(g_object->GetComponentByType(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	mesh->CalculateParentBoundingBox(mesh->parent);
	mesh->LoadBuffers();
	LOG("Loaded %s mesh successfully", file_name);
}

GLuint ModuleMesh::GetID()
{
	//Gen image
	ILuint img_id = 0;
	ilGenImages(1, &img_id);
	ilBindImage(img_id);
	return img_id;
}

std::string ModuleMesh::RandomName(aiMesh * mesh)
{
	std::string name = mesh->mName.C_Str();
	if (name.empty())
	{
		name = "GameObject";
		name += "_mesh_";
		name += std::to_string(rand_id);
		rand_id++;
	}
	else
	{
		name += std::to_string(rand_id);
		rand_id++;
	}
	return name;
}

bool ModuleMesh::SaveCurrentScene(const char * s_name)
{
	bool ret = true;

	//tag - num_objects - object

	uint size = sizeof(uint) * 2;

	std::list<std::pair<char*, uint>> object_buffers;
	std::vector<GameObject*> objects_to_save = App->scene->game_objects;


	//Saves all object data to buffers
	for (int i = 0; i < objects_to_save.size(); i++)
	{
		size += SaveGameObjects(object_buffers, objects_to_save[i]);
	}


	//Save tag
	char* data = new char[size];
	char* cursor = data;
	uint size_of = sizeof(uint);
	uint tag = 1;

	memcpy(cursor, &tag, size_of);
	cursor += size_of;

	//Save num objs
	uint num_objs = object_buffers.size();
	memcpy(cursor, &num_objs, size_of);
	cursor += size_of;


	//Stores all object data
	for (std::list<std::pair<char*, uint>>::iterator it = object_buffers.begin(); it != object_buffers.end(); it++)
	{
		size_of = (*it).second;
		memcpy(cursor, (*it).first, size_of);
		cursor += size_of;

		delete[](*it).first;
	}

	std::string file_path = App->file_system->CreateFolderInLibrary("Scenes");
	file_path += s_name;
	file_path += ".Amazing";

	//Create the file
	std::ofstream new_file(file_path.c_str(), std::ofstream::binary);

	if (new_file.good())
	{
		new_file.write(data, size);
		new_file.close();
	}
	delete[] data;

	LOG("Saved Scene %s to Library folder", s_name);


	return ret;
}

uint ModuleMesh::SaveGameObjects(std::list<std::pair<char*, uint>> &buffer, GameObject * g_object)
{
	//DATA ORDER: 
	// ID of obj - size of name - name -
	// ID of parent
	// ID of transform - pos - scale - rot 
	// num of meshes - ID of mesh - mesh name size - mesh name
	// ID of material - size of texture name - texture name
	// ID of camera - near dist - far dist

	//Calculate total size of data needed
	uint size = 1;

	size += sizeof(uint);
	std::string name = g_object->name;
	size += sizeof(uint);
	size += name.size();

	//ID parent
	size += sizeof(uint);

	//Transform component
	Transform* transform = dynamic_cast<Transform*>(g_object->GetComponentByType(COMPONENT_TYPE::COMPONENT_TRANSFORM));

	size += sizeof(uint);
	if (transform != nullptr)
	{
		size += sizeof(float) * 10; //position=3floats + scale=3floats + rotation=4floats
	}


	//Mesh component
	Geometry* mesh = dynamic_cast<Geometry*>(g_object->GetComponentByType(COMPONENT_TYPE::COMPONENT_MESH));

	size += sizeof(uint);//num of meshes
	if (mesh != nullptr)
	{
		size += sizeof(uint); //ID
		size += sizeof(uint); //size of mesh name
		size += sizeof(char) * mesh->name.length(); //mesh name
	}

	//Material component
	size += sizeof(uint);
	Image* mat = dynamic_cast<Image*>(g_object->GetComponentByType(COMPONENT_TYPE::COMPONENT_MATERIAL));
	if (mat != nullptr)
	{
		size += sizeof(uint);
		size += mat->p_tex.length();
	}

	//Camera component
	size += sizeof(uint);
	Camera* cam = dynamic_cast<Camera*>(g_object->GetComponentByType(COMPONENT_TYPE::COMPONENT_CAMERA));
	if (cam != nullptr)
	{
		size += sizeof(float) * 2; //near distance= 1float + far distance= 1float
	}

	//Now store data in memory
	char* data = new char[size];
	char* cursor = data;
	uint size_of = 0;


	//ID
	size_of = sizeof(uint);
	uint objID = g_object->ID;
	memcpy(cursor, &objID, size_of);
	cursor += size_of;

	//object name size
	size_of = sizeof(uint);
	uint name_size[] = { -1 };
	name_size[0] = name.size();
	memcpy(cursor, name_size, size_of);
	cursor += size_of;

	//object name
	size_of = name_size[0];
	memcpy(cursor, name.data(), size_of);
	cursor += size_of;

	//ID of parent
	uint parentID = 0;
	if(g_object->parent)
		parentID = g_object->parent->ID;
	size_of = sizeof(uint);
	memcpy(cursor, &parentID, size_of);
	cursor += size_of;

	//ID of transform
	uint transformID = 0;
	if (transform != nullptr)
		transformID = transform->ID;
	size_of = sizeof(uint);
	memcpy(cursor, &transformID, size_of);
	cursor += size_of;
	
	if (transform != nullptr)
	{
		//position
		math::float3 pos = transform->position;
		size_of = sizeof(float);
		memcpy(cursor, &pos.x, size_of);
		cursor += size_of;
		memcpy(cursor, &pos.y, size_of);
		cursor += size_of;
		memcpy(cursor, &pos.z, size_of);
		cursor += size_of;


		//scale
		math::float3 scale = transform->scale;
		memcpy(cursor, &scale.x, size_of);
		cursor += size_of;
		memcpy(cursor, &scale.y, size_of);
		cursor += size_of;
		memcpy(cursor, &scale.z, size_of);
		cursor += size_of;

		//rot
		math::Quat rot = transform->rot;
		size_of = sizeof(float);
		memcpy(cursor, &rot.x, size_of);
		cursor += size_of;
		size_of = sizeof(float);
		memcpy(cursor, &rot.y, size_of);
		cursor += size_of;
		size_of = sizeof(float);
		memcpy(cursor, &rot.z, size_of);
		cursor += size_of;
		size_of = sizeof(float);
		memcpy(cursor, &rot.w, size_of);
		cursor += size_of;
	}

	uint num_of_meshes = 0;		
	
	if (mesh != nullptr)
	{	
		num_of_meshes = 1;
	}
	size_of = sizeof(uint);
	memcpy(cursor, &num_of_meshes, size_of);
	cursor += size_of;

	if (mesh != nullptr)
	{
		//mesh ID
		uint meshID = 0;
		meshID = mesh->ID;
		size_of = sizeof(uint);
		memcpy(cursor, &meshID, size_of);
		cursor += size_of;

		//mesh name size
		uint size_of_name = mesh->name.length();
		size_of = sizeof(uint);
		memcpy(cursor, &size_of_name, size_of);
		cursor += size_of;

		//mesh name
		size_of = size_of_name;
		memcpy(cursor, mesh->name.data(), size_of);
		cursor += size_of;
	}
		
	//material ID
	uint matID = 0;
	if (mat != nullptr)
		matID = mat->ID;

	size_of = sizeof(uint);
	memcpy(cursor, &matID, size_of);
	cursor += size_of;

	if (mat != nullptr)
	{
		//texture name size
		uint texture_name_size = mat->p_tex.length();
		size_of = sizeof(uint);
		memcpy(cursor, &texture_name_size, size_of);
		cursor += size_of;

		//texture name
		size_of = texture_name_size;
		memcpy(cursor, mat->p_tex.data(), size_of);
		cursor += size_of;
	}

	//camera ID
	uint camID = 0;
	if (cam != nullptr)
		camID = cam->ID;

	size_of = sizeof(uint);
	memcpy(cursor, &camID, size_of);
	cursor += size_of;

	if (cam != nullptr)
	{
		//near and far plane
		float n_distance = cam->frustum.nearPlaneDistance;
		float f_distance = cam->frustum.farPlaneDistance;
		size_of = sizeof(float);
		memcpy(cursor, &n_distance, size_of);
		cursor += size_of;
		memcpy(cursor, &f_distance, size_of);
		cursor += size_of;
	}
	//add in our buffer the sieze and data
	data[size - 1] = '\0';
	std::pair<char*, uint> pair_of_data;
	buffer.push_back(pair_of_data);
	buffer.back().first = data;
	buffer.back().second = size;
 
	std::vector<GameObject*> children = g_object->children;
	for (int i = 0; i < children.size(); i++)
	{
		size += SaveGameObjects(buffer, children[i]);
	}

	return size;
}

GameObject * ModuleMesh::LoadSceneFromFormat(const char * s_name)
{
	char* data = nullptr;

	if (App->file_system->LoadDataFromLibrary(&data, s_name, "Scenes", ".Amazing") == false)
	{
		return nullptr;
	}

	//Reset the scene
	App->scene->RemoveSceneContent();

	//DATA ORDER: tag - num objects - [object]

	//Look if what we are loading is a correct scene saved
	char* cursor = data;
	uint tag[] = { -1 };
	uint size_of = sizeof(uint);
	memcpy(tag, cursor, size_of);

	if (*tag != 1)
	{
		LOG("ERROR: this is not a scene");
		return nullptr;
	}
	cursor += size_of;

	//num obj
	uint nums_objects[] = { -1 };
	size_of = sizeof(uint);
	memcpy(nums_objects, cursor, size_of);
	cursor += size_of;

	//Load back the data saved before to the obj
	std::vector<GameObject*> objects_in_scene;
	for (int i = 0; i < nums_objects[0]; i++)
	{
		objects_in_scene.push_back(LoadObjectFromFormat(cursor));
	}

	//Arrange the game objects in parents and childs
	for (uint i = 0; i < objects_in_scene.size(); ++i)
	{
		if (tmp_parent_ids[i] == 0)
			continue;
		GameObject* parent;
		for (int j = 0; j < objects_in_scene.size(); j++)
		{
			if (objects_in_scene[j] == objects_in_scene[i])
				continue;

			parent = objects_in_scene[j]->FindChildByID(tmp_parent_ids[i]);

			if (parent != nullptr)
			{
				objects_in_scene[i]->SetParent(parent);
				break;
			}
		}		
	}
	for (uint i = 0; i < objects_in_scene.size(); ++i)
	{
		if(objects_in_scene[i]->parent == nullptr)
			App->scene->game_objects.push_back(objects_in_scene[i]);
	}

	delete[] data;
	LOG("Loading scene %s", s_name);

	return nullptr;
}

GameObject* ModuleMesh::LoadObjectFromFormat(char *& cursor)
{
	//ORDER FOR LOADING THE OBJECT DATA
	// ID of obj - size of name - name -
	// ID of parent
	// ID of transform - pos - scale - rot 
	// num of meshes - ID of mesh - mesh name size - mesh name
	// ID of material - size of texture name - texture name
	// ID of camera - near distance - far distance
	
	//obj ID
	uint object_id = 0 ;
	uint size_of = sizeof(uint);
	memcpy(&object_id, cursor, size_of);
	cursor += size_of;

	//obj name size
	uint size_of_name;
	size_of = sizeof(uint);
	memcpy(&size_of_name, cursor, size_of);
	cursor += size_of;

	//obj name
	char* obj_name = new char[size_of_name + 1];
	size_of = size_of_name;
	memcpy(obj_name, cursor, size_of);
	cursor += size_of;
	obj_name[size_of_name] = '\0';

	//create the new obj and delete the now loaded name char
	GameObject* new_obj = new GameObject();
	new_obj->name = obj_name;
	new_obj->ID = object_id;
	delete[] obj_name;

	//parent id
	uint parent_id = 0 ;
	size_of = sizeof(uint);
	memcpy(&parent_id, cursor, size_of);
	cursor += size_of;
	tmp_parent_ids.push_back(parent_id);

	//transform ID
	uint transform_id =  0 ;
	size_of = sizeof(uint);
	memcpy(&transform_id, cursor, size_of);
	cursor += size_of;


	if (transform_id != 0)
	{
		//pos
		float pos[] = { 0, 0, 0 };
		size_of = sizeof(float) * 3;
		memcpy(pos, cursor, size_of);
		cursor += size_of;

		//scale
		float scale[] = { 1, 1, 1 };
		size_of = sizeof(float) * 3;
		memcpy(scale, cursor, size_of);
		cursor += size_of;

		//rot
		float rot[] = { 0, 0, 0 , 1 };
		size_of = sizeof(float) * 4;
		memcpy(rot, cursor, size_of);
		cursor += size_of;

		//create the transform component and prepare it to be used
		Transform* transform = dynamic_cast<Transform*>(new_obj->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		transform->ID = transform_id;
		transform->position.x = pos[0];
		transform->position.y = pos[1];
		transform->position.z = pos[2];
		transform->scale.x = scale[0];
		transform->scale.y = scale[1];
		transform->scale.z = scale[2];
		transform->rot.x = rot[0];
		transform->rot.y = rot[1];
		transform->rot.z = rot[2];
		transform->rot.w = rot[3];
		transform->rotation_matrix = math::float4x4::FromTRS(transform->position, transform->rot, transform->scale);
		transform->RotateObjects(new_obj);
	}

	// num mesh
	uint num_mesh = 0;
	size_of = sizeof(uint);
	memcpy(&num_mesh, cursor, size_of);
	cursor += size_of;

	if (num_mesh != 0)
	{
		//mesh ID
		uint mesh_id = 0;
		size_of = sizeof(uint);
		memcpy(&mesh_id, cursor, size_of);
		cursor += size_of;

		//mesh name size
		uint mesh_name_size = 0;
		size_of = sizeof(uint);
		memcpy(&mesh_name_size, cursor, size_of);
		cursor += size_of;

		//mesh name
		char* mesh_name = new char[mesh_name_size + 1];
		size_of = mesh_name_size;
		memcpy(mesh_name, cursor, size_of);
		cursor += size_of;

		mesh_name[mesh_name_size] = '\0';

		//load and prepare the mesh to be used and delete char data
		LoadMeshFromFormat(mesh_name, new_obj);//???
		delete[] mesh_name;
	}

	//ID of material
	uint mat_id = 0;
	size_of = sizeof(uint);
	memcpy(&mat_id, cursor, size_of);
	cursor += size_of;

	if (mat_id != 0)
	{
		//size texture name
		uint text_name_size = 0;
		size_of = sizeof(uint);
		memcpy(&text_name_size, cursor, size_of);
		cursor += size_of;

		//texture name
		char* text_name = new char[text_name_size + 1];
		size_of = text_name_size;
		memcpy(text_name, cursor, size_of);
		cursor += size_of;

		text_name[text_name_size] = '\0';

		bool is_loaded = false;
		uint texture_ID = -1;
		//Now we look for repeted texture use and only assign a new one if needed, if not assign the correct one
		for (std::vector<std::pair<std::string, int>>::iterator it = tmp_textures.begin(); it != tmp_textures.end(); it++)
		{
			if ((*it).first.compare(text_name) == 0)
			{
				is_loaded = true;
				texture_ID = (*it).second;
				break;
			}
		}

		if (is_loaded == false)
		{
			//Find texture path
			std::string path;
			#if _DEBUG
			path = "../Library/";
			#else
			path = "Library/";
			#endif

			path += "Textures/";
			path += text_name;
			path += ".dds";

			texture_ID = LoadImages(path.c_str(), true);

			//Save the new texture in our bank of textures
			std::pair<std::string, int> text;
			text.first = text_name;
			text.second = texture_ID;
			tmp_textures.push_back(text);
		}

		//Create the material component and prepare it to be used
		Image* material = dynamic_cast<Image*>(new_obj->CreateComponent(COMPONENT_TYPE::COMPONENT_MATERIAL));
		material->texture_id = texture_ID;
		material->tmp_id = material->texture_id;
		material->p_tex = text_name;
		material->ID = mat_id;
		material->LoadCheckerTexture();
		dynamic_cast<Geometry*>(new_obj->GetComponentByType(COMPONENT_TYPE::COMPONENT_MESH))->texture = material;
		delete[] text_name;
	}

	//camera ID
	uint cam_id = 0;
	size_of = sizeof(uint);
	memcpy(&cam_id, cursor, size_of);
	cursor += size_of;

	if (cam_id != 0)
	{
		//near dist and far dist
		float n_dist = 0, f_dist = 0;
		size_of = sizeof(float);
		memcpy(&n_dist, cursor, size_of);
		cursor += size_of;
		memcpy(&f_dist, cursor, size_of);
		cursor += size_of;

		//Create the camera component and prepare it to be used
		Camera* camera = dynamic_cast<Camera*>(new_obj->CreateComponent(COMPONENT_TYPE::COMPONENT_CAMERA));
		camera->frustum.nearPlaneDistance = n_dist;
		camera->frustum.farPlaneDistance = f_dist;
		camera->ID = cam_id;
	}

	cursor++;
	return new_obj;
}


void ModuleMesh::LoadTemporalMaterials(const aiScene * scene)
{
	//We get all the materials and we assign them -1, to later save them only the first time
	for (int i = 0; i < scene->mNumMaterials; i++)
	{
		std::pair<aiMaterial*, int> pair;
		pair.first = scene->mMaterials[i];
		pair.second = -1;
		tmp_material.push_back(pair);
	}
}

GLuint ModuleMesh::LoadImages(const char * p_tex, bool loading_scene)
{
	ILuint img_id = GetID();

	ilutRenderer(ILUT_OPENGL);

	//load from path
	//ilLoad(IL_TGA, p_tex);
	//const char* hola = "C:\\Users\\ivan_\\OneDrive\\Documentos\\GitHub\\AmazingEngine\\AmazingEngine\\Assets\\street\\building 01_c.tga";
	ilLoadImage(p_tex);

	ILuint devilError1 = ilGetError();
	if (devilError1 != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError1));
		return 0;
	}
	if (!loading_scene)
	{
		// If the image is flipped
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}
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
	//glGenTextures(1, &img_id);
	glBindTexture(GL_TEXTURE_2D, img_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
		0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
	glBindTexture(GL_TEXTURE_2D, 0);
	ILuint devilError3 = ilGetError();
	if (devilError3 != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError3));
		return 0;
	}
	//TODO: Get dimensions
	/*tex_dimension[0] = ilGetInteger(IL_IMAGE_WIDTH);
	tex_dimension[1] = ilGetInteger(IL_IMAGE_HEIGHT);*/

	return img_id;
}

void ModuleMesh::LoadTextureFromFormat(const char *, GameObject * g_object)
{

}

void ModuleMesh::LoadMaterials(const aiScene * scene, GameObject * g_object, const char* file_name, int last_mat_ind)
{
	Timer load;
	load.Start();
	int mat_id = 0;
	if (!tmp_material.empty())
	{
		uint check_id = 0;
		int tmp_id = 0;
		int texture_id = 0;
		std::string p_tex;
		if (scene->mMaterials[0]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString text_path;
			tmp_material[last_mat_ind].first->GetTexture(aiTextureType_DIFFUSE, 0, &text_path);
			if (text_path.length == 0)
				return;

			std::string  tex = text_path.C_Str();
			std::string  p_geo = file_name;
			
			std::reverse(tex.begin(), tex.end());

			while (tex.back() == '.' || tex.back() == '\\')
			{
				tex.pop_back();
			}
			std::reverse(tex.begin(), tex.end());
			//We change the name of the fbx for the texture name, with this made we have the general path
			while (p_geo.back() != '\\')
			{
				p_geo.pop_back();
			}
			p_geo += tex;
			p_tex = std::experimental::filesystem::path(tex).stem().string().c_str();

			//Look for if the texture has been already loaded
			if (tmp_material[last_mat_ind].second == -1)
			{
				texture_id = LoadImages(p_geo.c_str());
				tmp_id = texture_id;
				//Sending texture to our texture folder inside library folder
				ImportTextureToDDSFile(tex.c_str());

				tmp_material[last_mat_ind].second = texture_id;
				if (texture_id == 0)
				{
					LOG("No textures found in scene");
				}
			}
			else
			{
				texture_id = tmp_material[last_mat_ind].second;
				tmp_id = texture_id;
			}
		}
		else
			LOG("Cannot load this material");

		Image* tex = dynamic_cast<Image*>(g_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MATERIAL));
		tex->check_id = check_id;
		tex->tmp_id = tmp_id;
		tex->texture_id = texture_id;
		tex->p_tex = p_tex;
		tex->LoadCheckerTexture();
		dynamic_cast<Geometry*>(g_object->GetComponentByType(COMPONENT_TYPE::COMPONENT_MESH))->texture = tex;
		tex->to_delete;
		LOG("Loaded Material in : %i", load.Read());
	}
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
					tex->texture_id = LoadImages(file_name);
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

void ModuleMesh::DrawFrustums()
{
	glLineWidth(0.2);
	glColor3f(204, 255, 0.0f);

	while (c_frustums.empty() == false)
	{
		//We create the lines of the cube
		for (int i = 0; i < 4; i++)
		{
			glVertex3f(c_frustums.front()->CornerPoint(i + 4).x, c_frustums.front()->CornerPoint(i + 4).y, c_frustums.front()->CornerPoint(i + 4).z);
			glVertex3f(c_frustums.front()->CornerPoint(i).x, c_frustums.front()->CornerPoint(i).y, c_frustums.front()->CornerPoint(i).z);
		}
		//Create the vertices that define the cubes faces that form the frustum box
		for (int i = 0; i <= 4; i += 4)
		{
			glVertex3f(c_frustums.front()->CornerPoint(i).x, c_frustums.front()->CornerPoint(i).y, c_frustums.front()->CornerPoint(i).z);
			glVertex3f(c_frustums.front()->CornerPoint(i + 1).x, c_frustums.front()->CornerPoint(i + 1).y, c_frustums.front()->CornerPoint(i + 1).z);

			glVertex3f(c_frustums.front()->CornerPoint(i + 2).x, c_frustums.front()->CornerPoint(i + 2).y, c_frustums.front()->CornerPoint(i + 2).z);
			glVertex3f(c_frustums.front()->CornerPoint(i + 3).x, c_frustums.front()->CornerPoint(i + 3).y, c_frustums.front()->CornerPoint(i + 3).z);

			glVertex3f(c_frustums.front()->CornerPoint(i).x, c_frustums.front()->CornerPoint(i).y, c_frustums.front()->CornerPoint(i).z);
			glVertex3f(c_frustums.front()->CornerPoint(i + 2).x, c_frustums.front()->CornerPoint(i + 2).y, c_frustums.front()->CornerPoint(i + 2).z);

			glVertex3f(c_frustums.front()->CornerPoint(i + 1).x, c_frustums.front()->CornerPoint(i + 1).y, c_frustums.front()->CornerPoint(i + 1).z);
			glVertex3f(c_frustums.front()->CornerPoint(i + 3).x, c_frustums.front()->CornerPoint(i + 3).y, c_frustums.front()->CornerPoint(i + 3).z);
		}
		//We pop from the queue because we need to maintain update it for future changes in the gameobject's frustum
		c_frustums.pop();
	}
}

void ModuleMesh::DrawRay()
{
	glLineWidth(0.2);
	glColor3f(204, 255, 0.0f);
	float3 a = App->camera->ray_picking.a;
	float3 b = App->camera->ray_picking.b;
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(b.x, b.y, b.z);
}

void ModuleMesh::AddFrustumBox(math::Frustum * c_frustum)
{
	c_frustums.push(c_frustum);
}

