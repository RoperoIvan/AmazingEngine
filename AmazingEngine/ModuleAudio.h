#ifndef _MODULEAUDIO_H_
#define _MODULEAUDIO_H_

#include "Module.h"

class ModuleAudio : public Module
{
public:
	ModuleAudio(Application* app, bool start_enabled = true);
	~ModuleAudio();

	bool InitWwise();
	void ProcessAudio();
	void TermSoundEngine();
	//banks
	bool LoadBank(const char* path);
	void UnLoadBank(const char* path);

public:
	std::vector<std::string> banks;
};

#endif


