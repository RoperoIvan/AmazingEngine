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
