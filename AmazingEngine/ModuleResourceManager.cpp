#include "ModuleResourceManager.h"
#include "Application.h"

ModuleResourceManager::ModuleResourceManager(Application* app, bool start_enabled) : Module(app)
{
}

ModuleResourceManager::~ModuleResourceManager()
{
}

bool ModuleResourceManager::Init(const nlohmann::json * config_data)
{
	return false;
}

Resource * ModuleResourceManager::GetResource(uint id)
{
	Resource* res = nullptr;
	if (resources.count(id) >0)
		res = resources[id];

	return res;
}
