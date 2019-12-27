#include "ModuleAudio.h"
#include "Application.h"
#include "wwise_libraries.h"
#include "Wwise/include/Win32/AkFilePackageLowLevelIOBlocking.h"

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

ModuleAudio::ModuleAudio(Application* app, bool start_enabled) :Module(app, start_enabled)
{
}

ModuleAudio::~ModuleAudio()
{
}

update_status ModuleAudio::GameUpdate(float game_dt)
{
	ProcessAudio();
	return UPDATE_CONTINUE;
}

bool ModuleAudio::InitWwise()
{
	AkMemSettings memSettings;

	memSettings.uMaxNumPools = 20;

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)

	{
		assert(!"Could not create the memory manager.");

		return false;

	}
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);
	// Customize the Stream Manager settings here.
	if (!AK::StreamMgr::Create(stmSettings))
	{
		assert(!"Could not create the Streaming Manager");
		return false;
	}
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
	if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create the streaming device and Low-Level I/O system");
		return false;
	}

	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		assert(!"Could not initialize the Sound Engine.");
		return false;
	}
	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);
	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		assert(!"Could not initialize the Music Engine.");
		return false;
	}
#ifndef AK_OPTIMIZED
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);
	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		assert(!"Could not initialize communication.");
		return false;
	}
#endif // AK_OPTIMIZED

	return true;
}

void ModuleAudio::ProcessAudio()
{
	// Process bank requests, events, positions, RTPC, etc.
	AK::SoundEngine::RenderAudio();
}

void ModuleAudio::TermSoundEngine()
{
#ifndef AK_OPTIMIZED
	AK::Comm::Term();
#endif // AK_OPTIMIZED
	AK::MusicEngine::Term();
	AK::SoundEngine::Term();
	if (AK::IAkStreamMgr::Get())
		AK::IAkStreamMgr::Get()->Destroy();
	AK::MemoryMgr::Term();
}

bool ModuleAudio::LoadBank(const char* path)
{
	unsigned long bank_id;
	if (AK::SoundEngine::LoadBank(path, AK_DEFAULT_POOL_ID, bank_id))
		return false;

	banks.push_back(path);
	return true;
}

void ModuleAudio::UnLoadBank(const char* path)
{
	AK::SoundEngine::UnloadBank(path, NULL);
	for (uint i = 0; i < banks.size(); ++i)
	{
		if (banks[i].compare(path) == 0)
		{
			banks.erase(banks.begin() + i);
			break;
		}
	}
}

void ModuleAudio::CreateEmmiter(char* name, float3 position)
{
	SoundEmmiter* emmiter = new SoundEmmiter(new_id, name);
	++new_id;
	emmiter->SetPosition(position, float3(position.x + 1, position.y, position.z), float3(position.x, position.y + 1, position.z));
	emmiters.push_back(emmiter);
}

void ModuleAudio::DeleteEmmiter(unsigned int id)
{
	for (uint i = 0; i < emmiters.size(); ++i)
	{
		SoundEmmiter* emmiter = nullptr;
		if (emmiters[i]->GetID() == id)
		{
			emmiter = emmiters[i];
			emmiters.erase(emmiters.begin() + i);
			delete emmiter;
			emmiter = nullptr;
			break;
		}
	}
}
void ModuleAudio::DeleteEmmiter(SoundEmmiter* emm)
{
	for (uint i = 0; i < emmiters.size(); ++i)
	{
		SoundEmmiter* emmiter = nullptr;
		if (emmiters[i] == emm)
		{
			emmiter = emmiters[i];
			emmiters.erase(emmiters.begin() + i);
			delete emmiter;
			emmiter = nullptr;
			break;
		}
	}
}

void ModuleAudio::StopSounds() const
{
	AK::SoundEngine::StopAll();
}
void ModuleAudio::PauseSounds() const
{
	AK::SoundEngine::PostEvent("Pause_All", AK_INVALID_GAME_OBJECT);
}

void ModuleAudio::ResumeSounds() const
{
	AK::SoundEngine::PostEvent("Resume_All", AK_INVALID_GAME_OBJECT);
}
