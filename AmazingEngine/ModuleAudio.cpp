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
