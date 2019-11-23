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
	bool LoadDataFromLibrary(char ** data, const char * name, const char * directory, const char * extension) const;
	char* DataValue(char*& cursor, std::string info, int size, char* end = ";");
	char* DataValue(std::string& cursor, std::string info, int size, std::string end = ";");
	void SaveTextureToDDS(const char* data, uint size, const char* name);
	bool SaveMeshToFormat(const char* name, uint num_vert, uint num_ind, const float* vert, uint* ind, const float* normals, uint num_normals, uint num_face_normals, const float* texture_coords, uint num_coords, uint id_coords);
	std::string CreateFolderInLibrary(const char* folder);

private:
	GameObject* GetParentbyID(const int& ID);
public:
	std::vector<GameObject*> import_objects;

};

#endif
