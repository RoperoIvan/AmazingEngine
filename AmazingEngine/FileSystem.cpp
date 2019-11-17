#include "FileSystem.h"
#include <fstream>
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
		}
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


		App->scene->game_objects.push_back(new_object);

	}
	return true;
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
	int pos = cursor.find(info) + sizeof(char) * (info.size() + 1);

	int final_pos = cursor.find(end, sizeof(char) * info.size()) - sizeof(char) * (end.size() + 1) ;

	std::string value = cursor.substr(pos, final_pos);

	std::string a(cursor.substr(pos));
	if(!a.empty())
		cursor.assign(a);
	char* final_value = new char[value.size()];
	strcpy(final_value, value.c_str());
	return final_value;
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
