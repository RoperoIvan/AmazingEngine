// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer()
{
	Start();
}

// ---------------------------------------------
void Timer::Start()
{
	running = true;
	started_at = SDL_GetTicks();
	state = TIMER_STATE::START;
}

// ---------------------------------------------
void Timer::Stop(TIMER_STATE state)
{
	running = false;
	stopped_at = SDL_GetTicks() - started_at;
	this->state = state;
}

void Timer::Resume()
{
	running = true;
	started_at = SDL_GetTicks() - stopped_at;
	state = TIMER_STATE::START;

}

void Timer::SetState(TIMER_STATE state)
{
	this->state = state;
}

TIMER_STATE Timer::GetState()
{
	return state;
}

// ---------------------------------------------
Uint32 Timer::Read()
{
	if(running == true)
	{
		return SDL_GetTicks() - started_at;
	}
	else
	{
		return stopped_at;
	}
}


