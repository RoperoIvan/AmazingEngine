#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

enum class TIMER_STATE
{
	START,
	STOP,
	PAUSE
};

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Stop(TIMER_STATE state = TIMER_STATE::STOP);
	void Resume();
	void SetState(TIMER_STATE state);

	TIMER_STATE GetState();
	Uint32 Read();

private:

	bool	running;
	Uint32	started_at;
	Uint32	stopped_at;
	Uint32	time_in_pause;
	TIMER_STATE state;
};

#endif //__TIMER_H__