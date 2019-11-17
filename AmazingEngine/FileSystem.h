#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include "Module.h"
#include <vector>
#include "GameObject.h"

class FileSystem: public Module
{
public:
	FileSystem(Application* app, bool start_enabled = true);
	~FileSystem();

	bool SaveScene(char* path, std::vector<GameObject*> objects);
	bool ImporScene(char* path);

	char* DataValue(char*& cursor, std::string info, int size, char* end = ";");
	char* DataValue(std::string& cursor, std::string info, int size, std::string end = ";");

private:
	GameObject* GetParentbyID(const int& ID);
public:
	std::vector<GameObject*> import_objects;

};

#endif
