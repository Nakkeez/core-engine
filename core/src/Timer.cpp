#include "../include/Timer.h"

#if defined (_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#if defined (_LINUX)
#include <time.h>
#include <sys/time.h>
#endif


Timer::Timer() :
	m_dRateToSeconds(0.0),
	m_uTickFrequency(0),
	m_uStartClock(0),
	m_fElapsedSeconds(0.0f)
{
#if defined (_WINDOWS)
	uint64_t rate;
	// Get frequency of CPU ticks (ticks/second)
	::QueryPerformanceFrequency((LARGE_INTEGER*)&rate);
	m_dRateToSeconds = 1.0 / (double)rate; // Get inverse number
	m_uTickFrequency = rate;
#endif

#if defined (_LINUX)
	m_dRateToSeconds = 1.0 / 1000000000.0;
	m_uTickFrequency = 10000000;
#endif
}


void Timer::BeginTimer()
{
	m_uStartClock = GetTicks();
}


void Timer::EndTimer()
{
	const uint64_t endClock = GetTicks();
	// Check if BeginTimer has been called
	if (endClock == m_uStartClock || m_uStartClock == 0)
	{
		// Frametime is used as divider for other values so it can't it be zero
		m_fElapsedSeconds = 0.000001f;
	}
	else
	{
		const double ticks = (double)(endClock - m_uStartClock);
		m_fElapsedSeconds = (float)(ticks * m_dRateToSeconds);
	}
}


uint64_t Timer::GetTicks()
{
	uint64_t ret = 0;

#if defined (_WINDOWS)
	// Get the amount of CPU ticks
	::QueryPerformanceCounter((LARGE_INTEGER*)&ret);

#elif defined (_LINUX)
	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC_RAW, &time);

	static long long timevalsub = 0;
	if (timevalsub == 0)
	{
		timevalsub = (long long)time.tv_sec;
	}
	time.tv_sec -= timevalsub;
	ret = (time.tv_sec * 1000000000llu + time.tv_nsec);
#endif

	return ret;
}
