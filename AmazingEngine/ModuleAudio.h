#ifndef _MODULEAUDIO_H_
#define _MODULEAUDIO_H_

#include "Module.h"
#include "SoundEmmiter.h"

class ModuleAudio : public Module
{
public:
	ModuleAudio(Application* app, bool start_enabled = true);
	~ModuleAudio();

	update_status GameUpdate(float game_dt);

	bool InitWwise();
	void ProcessAudio();
	void TermSoundEngine();
	//banks
	bool LoadBank(const char* path);
	void UnLoadBank(const char* path);
	void CreateEmmiter(char* name, float3 position);

	void DeleteEmmiter(unsigned int id);

	void DeleteEmmiter(SoundEmmiter* emm);

	void StopSounds() const;

	void PauseSounds() const;

	void ResumeSounds() const;

public:
	std::vector<std::string> banks;
	std::vector<SoundEmmiter*> emmiters;

private:
	int new_id = 1;
};

#endif


