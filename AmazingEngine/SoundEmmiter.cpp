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

void SoundEmmiter::SetPosition(float3 pos, float3 frontt, float3 up)
{
	position.X = pos.x;
	position.Y = pos.y;
	position.Z = pos.z;
	top.X = up.x / up.Length();
	top.Y = up.y / up.Length();
	top.Z = up.z / up.Length();
	front.X = frontt.x / frontt.Length();
	front.Y = frontt.y / frontt.Length();
	front.Z = frontt.z / frontt.Length();

	AkSoundPosition sound_pos;
	sound_pos.Set(position, front, top);
	AKRESULT res = AK::SoundEngine::SetPosition((AkGameObjectID)id, sound_pos);
}

int SoundEmmiter::GetID()
{
	return id;
}

void SoundEmmiter::PlayEvent(unsigned long id)const
{
	AK::SoundEngine::PostEvent(id, this->id);
}

void SoundEmmiter::PlayEvent(const char* name)const
{
	AK::SoundEngine::PostEvent(name, this->id);
}

void SoundEmmiter::StopEvent(const char* name)const
{
	AK::SoundEngine::ExecuteActionOnEvent(name, AK::SoundEngine::AkActionOnEventType_Stop);
}