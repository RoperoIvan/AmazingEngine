#include "SoundEmmiter.h"
#include "wwise_libraries.h"

SoundEmmiter::SoundEmmiter(unsigned int id, char* name)
{
	this->id = id;
	this->name = name;
	AK::SoundEngine::RegisterGameObj((AkGameObjectID)id, name);
}

SoundEmmiter::~SoundEmmiter()
{
	AK::SoundEngine::UnregisterGameObj(id);
}
