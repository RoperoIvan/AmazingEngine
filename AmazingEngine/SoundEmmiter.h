#ifndef _SOUNDEMMITER_H_
#define _SOUNDEMMITER_H_

#include "Wwise/include/AK/SoundEngine/Common/AkTypes.h"

class SoundEmmiter
{
public:
	SoundEmmiter(unsigned int id, const char* name);
	~SoundEmmiter();

private:
	char* name;
	unsigned int id;
	unsigned int volum;

	AkVector position;
	AkVector orient_top;
	AkVector orient_front;
};

#endif
