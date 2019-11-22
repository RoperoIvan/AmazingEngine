#include "FileSystem.h"
#include <fstream>
#include <experimental\filesystem>
#include "Application.h"


FileSystem::FileSystem(Application* app, bool start_enabled):Module(app)
{
}

FileSystem::~FileSystem()
{
}

bool FileSystem::SaveScene(char* path, std::vector<GameObject*>objects)
{
	bool ret = true;
	FILE* file;
	file = std::fopen(path, "wt");

	if (file == NULL)
	{
		return false;
	}
	for (std::vector<GameObject*>::iterator object = objects.begin(); object != objects.end(); ++object)
	{
		if (*object != nullptr)
		{
			(*object)->SaveMesh(file);
			//(*object)->SaveMaterial(file);
			/*App->mesh->ImportTextureToDDSFile()*/
		}
		//TODO: SAVE TEXTURE TOO

	}
	std::fclose(file);
	return ret;
}

bool FileSystem::ImporScene(char* path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		file.open(path);
	}
	//save all scene in data
	uint i = 0;
	std::string data;
	while (!file.eof())
	{
		char character;
		file >> character;
		data.push_back(character);
		++i;
	}

	file.close();
	//craete a cursor to read data
	char* cursor = (char*)data.data();
	while (cursor <= cursor + sizeof(char)*data.size())
	{
		cursor = strstr(cursor, "GameObject:");

		if (cursor == NULL)
			break;

		
		GameObject* new_object = new GameObject();

		//save parent ID
		std::stringstream convertor(DataValue(cursor, "parentID:", 10));
		int parentID;
		convertor >> parentID;

		//save game object data
		char* end_object = strstr(cursor, "//");
		new_object->ImportMesh(cursor, end_object);


		//if don't have parent create new object else search parent
		if (parentID == 0)
		{
			import_objects.push_back(new_object);
		}
		else
		{
			GameObject* parent_object = GetParentbyID(parentID);
			if (parent_object != nullptr)
				parent_object->children.push_back(new_object);
		}
	}
	for(std::vector<GameObject*>::iterator iter = import_objects.begin(); iter != import_objects.end(); ++iter)
		App->scene->game_objects.push_back(*iter);
	return true;
}

bool FileSystem::LoadDataFromLibrary(char ** data, const char * name, const char * directory, const char * extension) const
{
	std::string path;
#if _DEBUG
	path = "../Library/";
#else
	path = "Library/";
#endif

	path += directory;
	path += '/';
	path += name;
	path += extension;

	//Search file
	std::ifstream file(path, std::ifstream::binary);

	//Get file length
	file.seekg(0, file.end);
	std::streamsize length = file.tellg();
	file.seekg(0, file.beg);

	//Load data to buffer-----------------------------------------
	if (file.good() && file.is_open())
	{
		data[0] = new char[length];
		file.read(data[0], length);
		file.close();
		return true;
	}
	else
	{
		LOG("ERROR: could not load mesh %s.Amazing", name);
		return false;
	}
}

//search string in data and return to next ; or size
char* FileSystem::DataValue(char*& cursor, std::string info, int size, char* end)
{
	cursor = strstr(cursor, info.c_str());
	std::string aux;
	cursor += sizeof(char) * info.size();
	char* limit = strstr(cursor, end);

	while (size > 0)
	{
		aux.push_back(*cursor);

		 if (cursor == limit)
			 break;
		++cursor;
		--size;
	}
	char* value = new char[aux.size()];
	strcpy(value, aux.c_str());
	return value;
}

char* FileSystem::DataValue(std::string& cursor, std::string info, int size, std::string end)
{
	if (cursor.find(info) == std::string::npos)
		return";";
	int pos = cursor.find(info) + sizeof(char) * (info.size());

	int final_pos = cursor.find(end, pos + sizeof(char) * info.size()) - sizeof(char) * (end.size()) ;

	std::string value = cursor.substr(pos, final_pos);

	std::string a(cursor.substr(pos));
	if(!a.empty())
		cursor.assign(a);
	char* final_value = new char[value.size()];
	strcpy(final_value, value.c_str());
	return final_value;
}

void FileSystem::SaveTextureToDDS(const char* data, uint size, const char* name)
{
	std::string file_path = CreateFolderInLibrary("Textures");
	file_path += (std::experimental::filesystem::path(name).stem().string());
	file_path += ".dds";

	//Write all to new file
	std::ofstream new_file(file_path.c_str(), std::ofstream::binary);

	if (new_file.good())
	{
		new_file.write(data, size);
		new_file.close();
		LOG("Saved texture %s to Library", name);
	}
	else
	{
		LOG("ERROR: Could not save texture to .DDS");
	}
}

bool FileSystem::SaveMeshToFormat(const char * name, uint num_vert, uint num_ind, const float * vert, uint * ind, const float * normals, uint num_normals, uint num_face_normals, const float* texture_coords, uint num_coords, uint id_coords)
{
	bool ret = true;

	//DATA ORDER: num vertex - num index - vertex - index - has normals - has text coords - normals - text coords - num_normals - num_face_normals - num_coords - id_coords

	bool has_normals = false;
	bool has_text_coords = false;


	//calculate base size
	uint size = sizeof(uint) * 7 + sizeof(float) * num_vert * 3 + sizeof(uint) * num_ind + sizeof(bool) * 2 + 1;

	if (normals)
	{
		has_normals = true;
		size += sizeof(float)*num_vert * 3;
	}
	if (texture_coords)
	{
		has_text_coords = true;
		size += sizeof(float)*num_vert * 2;
	}


	char* data = new char[size];
	char* cursor = data;

	data[size - 1] = '\0';
	uint ranges[7] = { 02, num_vert, num_ind,  num_normals, num_face_normals, num_coords,  id_coords};
	uint size_of = sizeof(ranges);


	//Copy num vert & num ind
	memcpy(cursor, ranges, size_of);
	cursor += size_of;


	//Copy vert
	size_of = sizeof(float)*num_vert * 3;
	memcpy(cursor, vert, size_of);
	cursor += size_of;

	//Copy ind
	size_of = sizeof(uint)*num_ind;
	memcpy(cursor, ind, size_of);
	cursor += size_of;

	//Copy normals and text coords
	bool has[2] = { has_normals, has_text_coords };

	size_of = sizeof(bool);
	memcpy(cursor, &has[0], size_of);
	cursor += size_of;

	if (has_normals)
	{
		size_of = sizeof(float)*num_vert * 3;
		memcpy(cursor, normals, size_of);
		cursor += size_of;
	}

	size_of = sizeof(bool);
	memcpy(cursor, &has[1], size_of);
	cursor += size_of;

	if (has_text_coords)
	{
		size_of = sizeof(float)*num_vert * 2;
		memcpy(cursor, texture_coords, size_of);
		cursor += size_of;
	}

	std::string file_path = CreateFolderInLibrary("Meshes");
	file_path += name;
	file_path += ".Amazing";

	//Write all to new file
	std::ofstream new_file(file_path.c_str(), std::ofstream::binary);

	if (new_file.good())
	{
		new_file.write(data, size);
		new_file.close();
	}
	else
	{
		LOG("ERROR: Could not save file to .Amazing");
		ret = false;
	}

	delete[] data;

	return ret;
}

std::string FileSystem::CreateFolderInLibrary(const char * folder)
{
	std::string path;

#if _DEBUG
	path = "..\\Library";

#else 
	path = ".\\Library";
#endif

	CreateDirectory(path.c_str(), NULL);
	SetFileAttributes(path.c_str(), FILE_ATTRIBUTE_HIDDEN);

	path += "\\";
	path += folder;

	CreateDirectory(path.c_str(), NULL);
	SetFileAttributes(path.c_str(), FILE_ATTRIBUTE_HIDDEN);

	path += "\\";

	return path;
}

GameObject* FileSystem::GetParentbyID(const int& ID)
{
	for (std::vector<GameObject*>::iterator iter = import_objects.begin(); iter != import_objects.end(); ++iter)
	{
		if ((*iter)->ID == ID)
			return *iter;
	}
	return nullptr;
}
