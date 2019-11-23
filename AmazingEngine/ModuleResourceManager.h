#ifndef _MODULERESOURCEMANAGER_H
#define _MODULERESOURCEMANAGER_H

#include "Module.h"
#include <map>

class Resource;

class ModuleResourceManager : public Module
{
public:
	ModuleResourceManager(Application* app, bool start_enabled = true);
	~ModuleResourceManager();

	bool Init(const nlohmann::json* config_data);

	Resource* GetResource(uint id);

private:

	std::map<uint, Resource*> resources;

};

#endif