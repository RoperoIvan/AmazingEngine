#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "Component.h"

class Audio : public Component
{
public:
	Audio(GameObject* parent);
	~Audio();
	virtual void Enable();
	virtual void Update() = 0;
	virtual void Disable();

};

#endif