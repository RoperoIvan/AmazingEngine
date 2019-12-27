#ifndef _SOUNDEMMITER_H_
#define _SOUNDEMMITER_H_

#include "Wwise/include/AK/SoundEngine/Common/AkTypes.h"
#include "MathGeoLib\include\Math\float3.h"

class SoundEmmiter
{
public:
	SoundEmmiter(unsigned int id, char* name);
	~SoundEmmiter();

	void SetPosition(float3 position, float3 front, float3 up);
	int GetID();

	void PlayEvent(unsigned long id) const;
	void PlayEvent(const char* name) const;
	void StopEvent(const char* name) const;

private:
	char* name;
	unsigned int id;
	unsigned int volum;

	AkVector position;
	AkVector top;
	AkVector front;
};

#endif
