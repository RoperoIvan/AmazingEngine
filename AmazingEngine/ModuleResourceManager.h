#ifndef _MODULERESOURCEMANAGER_H
#define _MODULERESOURCEMANAGER_H

#include "Module.h"
#include <map>
#include "Resource.h"

class ModuleResourceManager : public Module
{
public:
	ModuleResourceManager(Application* app, bool start_enabled = true);
	~ModuleResourceManager();

	bool Init(const nlohmann::json* config_data);
	bool CleanUp();
	Resource* GetResource(uint id);
	Resource* CreateResource(Resource::TYPE type);	

private:

	std::map<uint, Resource*> resources;

};

#endif