#pragma once

#include <cstdint>

class Timer
{
public:
	Timer();

	/**
	 * Start calculating the time
	 */
	void BeginTimer();

	/**
	 * Stop calculatig the time
	 */
	void EndTimer();

	/**
	 * Get time between intervals
	 * @return seconds between last BeginTimer - EndTimer calls.
	 */
	inline float GetElapsedSeconds() const { return m_fElapsedSeconds; }

	/**
	 * Get CPU ticks to make the timer as precise as possible
	 * @return system timer ticks
	 */
	static uint64_t GetTicks();

private:
	double		m_dRateToSeconds;
	uint64_t	m_uTickFrequency;
	uint64_t	m_uStartClock;

	float		m_fElapsedSeconds;
};