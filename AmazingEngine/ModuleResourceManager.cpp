#include "ModuleResourceManager.h"
#include "Application.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"


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

bool ModuleResourceManager::CleanUp()
{
	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		delete it->second;
	}

	return true;
}

Resource * ModuleResourceManager::GetResource(uint id)
{
	Resource* res = nullptr;
	if (resources.count(id) >0)
		res = resources[id];

	return res;
}

Resource * ModuleResourceManager::CreateResource(Resource::TYPE type)
{
	Resource* resource = nullptr;
	LCG rand_gen;
	uint UID = rand_gen.Int();
	switch (type)
	{
	case Resource::MATERIAL: 
		resource = (Resource*) new ResourceMaterial(UID);
		break;
	case Resource::MESH:
		resource = (Resource*) new ResourceMesh(UID);
		break;
	}

	if (resource != nullptr)
		resources[UID] = resource;

	return resource;
}
