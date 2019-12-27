#include "Audio.h"
#include "Application.h"
#include "ModuleAudio.h"

Audio::Audio(GameObject* parent):Component(parent,COMPONENT_TYPE::COMPONENT_AUDIO)
{
	App->audio->CreateEmmiter((char*)parent->name.c_str(), dynamic_cast<Transform*>(parent->GetComponentByType(COMPONENT_TYPE::COMPONENT_TRANSFORM))->position);
}

Audio::~Audio()
{
}

void Audio::Enable()
{
}

void Audio::Disable()
{
}
