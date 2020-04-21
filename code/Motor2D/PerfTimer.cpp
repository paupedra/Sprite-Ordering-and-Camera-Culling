// ----------------------------------------------------
// PerfTimer.cpp
// Slow timer with microsecond precision
// ----------------------------------------------------

#include "PerfTimer.h"
#include "SDL\include\SDL_timer.h"

uint64 PerfTimer::frequency = 0;

// ---------------------------------------------
PerfTimer::PerfTimer()
{
	if (frequency == 0)
	{
		frequency = SDL_GetPerformanceFrequency();									//Sets the frequency cycles in seconds (coming from microsenconds).
	}
	
	Start();
}

// ---------------------------------------------
void PerfTimer::Start()
{
	started_at = SDL_GetPerformanceCounter();										//Initializes the timer in microseconds.	
}

// ---------------------------------------------
double PerfTimer::ReadMs() const
{
	return ((SDL_GetPerformanceCounter() - started_at)) / (frequency / 1000.0f);	//Checks how many miliseconds have passed since the simer started (Start()). The first part is in microseconds and the second one is in seconds, so they need to be translated.
}

// ---------------------------------------------
uint64 PerfTimer::ReadTicks() const
{
	return SDL_GetPerformanceCounter() - started_at;	//Checks how many ticks (in microseconds) have passed since the timer started (Start()).
}